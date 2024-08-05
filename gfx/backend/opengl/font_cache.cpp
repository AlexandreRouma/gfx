#include "font_cache.h"
#include "flog/flog.h"
#include <stdexcept>
#include <fstream>
#include <format>

namespace gfx::OpenGL {
    FT_Library FontCache::library;
    bool FontCache::isInit = false;

    FontCache::FontCache(const std::function<void(int)>& bindTexture) : atlas(bindTexture) {
        // Init freetype if it isn't already initialized
        initFreetype();
    }

    FontMetrics FontCache::getFontMetrics(const Font& font) {
        // Get the font data
        FontData& data = *(std::any_cast<FontData*>(font.backendTag));

        // Return the metrics
        return data.metrics;
    }

    void FontCache::loadFont(const std::string& path) {
        // Open the font file
        std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

        // Make sure it opened successfully
        if (!file.is_open()) {
            throw std::runtime_error("Could not open font file");
        }

        // Get its length
        size_t len = file.tellg();
        file.seekg(0);

        // Allocate the buffer
        uint8_t* data = new uint8_t[len];

        // Load the font data into the buffer
        file.read((char*)data, len);

        // Close the font file
        file.close();

        // Load font data into Freetype
        FT_Face face;
        FT_New_Memory_Face(library, data, len, 0, &face);

        // Make sure a font with that name is not already loaded
        std::string name = std::format("{} {}", face->family_name, face->style_name);
        if (fontFiles.find(name) != fontFiles.end()) { return; }
        flog::debug("Loaded font: '{}'", name);

        // TODO: The font file data list should be global to all font caches
        // Actually a lot of objects should be global to avoid reloading in multiple windows
        // Only the OpenGL objects, and thus atlas should belong to each instance

        // Destroy freetype data
        FT_Done_Face(face);

        // Create an entry in the file list
        FontFile ff = { data, len };
        fontFiles[name] = ff;
    }

    void FontCache::prepareFont(Font& font) {
        // If the tag is already filled in, do nothing
        if (font.backendTag.has_value()) { return; }

        // Search for the font in the cache and add it if it's not available
        auto it = fonts.find(font);
        if (it == fonts.end()) {
            admitFont(font);
            it = fonts.find(font);
        }

        // Set the tag to the pointer to the font data
        font.backendTag = &it->second;
    }

    GlyphInfo FontCache::getGlyph(const Font& font, int glyphId, int alignment) {
        // Get the font data
        FontData& data = *(std::any_cast<FontData*>(font.backendTag));

        // Create the descriptor
        GlyphDescriptor desc = (glyphId << 2) | alignment;

        // If the glyph is cached, return its info immediately
        auto itg = data.glyphs.find(desc);
        if (itg != data.glyphs.end()) {
            return itg->second;
        }

        // The glyph is not in cache, add it
        return admitGlyph(data, desc);
    }

    Vec2f FontCache::getKerning(const Font& font, int leftId, int rightId) {
        // Get the font data
        FontData& data = *(std::any_cast<FontData*>(font.backendTag));

        // Get the kerning info from Freetype
        FT_Vector delta;
        FT_Get_Kerning(data.face, leftId, rightId, FT_KERNING_UNFITTED, &delta);
        
        // Convert it to floating point and return it
        return Vec2f((float)delta.x / (float)(1 << 6), (float)delta.y / (float)(1 << 6));
    }

    void FontCache::initFreetype() {
        // If already initialized, do nothing
        if (isInit) { return; }

        // Initialize FreeType
        int err = FT_Init_FreeType(&library);
        if(err) {
            throw std::runtime_error("Could not initialize FreeType");
        }
    }

    void FontCache::admitFont(const Font& font) {
        // Get the font and throw an error if not available
        auto it = fontFiles.find(font.getName());
        if (it == fontFiles.end()) {
            throw std::runtime_error("The requested font is not loaded");
        }
        const FontFile& file = it->second;

        // Load font data into Freetype
        FT_Face face;
        FT_New_Memory_Face(library, file.data, file.size, 0, &face);

        // Set font size
        FT_Set_Pixel_Sizes(face, 0, font.getSize());

        // Compute font metrics
        FontMetrics metrics = {
            (float)face->size->metrics.ascender / (float)(1 << 6),
            (float)face->size->metrics.descender / (float)(1 << 6)
        };

        // Create the data struct
        FontData data = {
            face,
            face->glyph,
            metrics,
            std::map<GlyphDescriptor, GlyphInfo>()
        };

        // Add the data to the cache
        fonts[font] = data;
    }

    void FontCache::evictFont(const Font& font) {
        // TODO
    }

    GlyphInfo FontCache::admitGlyph(FontData& font, GlyphDescriptor desc) {
        // Apply subpixel alignment
        FT_Vector delta;
        delta.x = (float)(desc & 0b11) * (64.0f/4.0f);
        delta.y = 0;
        FT_Set_Transform(font.face, NULL, &delta);

        // Render the glyph
        FT_Load_Char(font.face, desc >> 2, FT_LOAD_RENDER);

        // Add to the atlas
        GlyphCords coords;
        Size glyphSize = Size(font.slot->bitmap.width, font.slot->bitmap.rows);
        atlas.addGlyph(glyphSize, font.slot->bitmap.buffer, coords);

        // Create cache entry
        float texSize = atlas.getTextureSize();
        GlyphInfo info = {
            glyphSize,
            Vec2f(font.slot->bitmap_left, font.slot->bitmap_top),
            coords,
            atlas.getTextureID(),
            (float)font.face->glyph->linearHoriAdvance * (1.0f / (float)(1 << 16)),
            -1
        };

        // Push entry to the cache
        font.glyphs[desc] = info;

        // Return glyph info
        return info;
    }

    void FontCache::evictGlyph(FontData& font, GlyphDescriptor desc) {
        // TODO
    }
}