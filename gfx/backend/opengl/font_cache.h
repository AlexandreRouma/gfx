#pragma once
#include "font_atlas.h"
#include "../../types.h"
#include "../../font.h"
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

#define GFX_OPENGL_GLYPH_SUBPIXELS  4

namespace gfx::OpenGL {
    struct FontFile {
        uint8_t* data;
        size_t size;
    };

    struct FontMetrics {
        float ascender;
        float descender;
    };

    struct GlyphInfo {
        // Glyph geometry
        Sizei size;
        Vec2i offset;

        // Texture location
        GlyphCords coords;
        GLuint textureId;

        // Advance instructions
        float xAdvance;

        // Usage statistics
        int refCount;
    };

    typedef uint32_t GlyphDescriptor;

    struct FontData {
        FT_Face face;
        FT_GlyphSlot slot;
        FontMetrics metrics;
        std::map<GlyphDescriptor, GlyphInfo> glyphs;
    };

    struct GlyphPair {
        int leftId;
        int rightId;

        inline bool operator==(const GlyphPair& b) const {
            return leftId == b.leftId && rightId == b.rightId;
        }
    };

    class FontCache {
    public:
        FontCache(const std::function<void(int)>& bindTexture);

        /**
         * Load a font from file.
         * @param path Path to the font file.
        */
        void loadFont(const std::string& path);

        /**
         * Notify that cache that a font is about to be used so that its tag can be filled in for fast access.
         * @param font Font to prepare.
        */
       void prepareFont(Font& font);

        /**
         * Get the metrics of a font. The font must have been prepared first.
         * @param font Font to get the metrics of.
         * @return Metrics of the font.
        */
        FontMetrics getFontMetrics(const Font& font);

        /**
         * Get a glyph from a font by ID and alignement. Can only be called once OpenGL is set up. The font must have been prepared first.
         * @param font Font to which the glyphs belong.
         * @param glyphId Unicode ID of the glyph.
         * @param alignment Sub-pixel alignement. Must be between 0 and 3 inclusive.
        */
        GlyphInfo getGlyph(const Font& font, int glyphId, int alignment);

        /**
         * Get the kerning information for two adjacent glyphs by their IDs. Can only be called once OpenGL is set up. The font must have been prepared first.
         * @param font Font to which the glyphs belong.
         * @param leftId Unicode ID of the left glyph.
         * @param rightId Unicode ID of the right glyph.
        */
        Vec2f getKerning(const Font& font, int leftId, int rightId);

        FontAtlas atlas;

    private:
        static void initFreetype();

        void admitFont(const Font& font);
        void evictFont(const Font& font);
        
        GlyphInfo admitGlyph(FontData& font, GlyphDescriptor desc);
        void evictGlyph(FontData& font, GlyphDescriptor desc);

        std::unordered_map<std::string, FontFile> fontFiles;
        std::unordered_map<Font, FontData> fonts;

        static bool isInit;
        static FT_Library library;
    };
}