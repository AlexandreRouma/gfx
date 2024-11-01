#include "font_atlas.h"

#define FONT_ATLAS_MAX_SIZE 512

namespace gfx::OpenGL {
    FontAtlas::FontAtlas(const std::function<void(int)>& bindTexture) {
        // Save texture bind function
        this->bindTexture = bindTexture;

        // Determine size to use
        int maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        texSize = std::min<int>(FONT_ATLAS_MAX_SIZE, maxTextureSize);

        // Allocate the CPU-side texture
        int elemCount = texSize*texSize;
        bitmap = new uint32_t[elemCount];
        for (int i = 0; i < elemCount; i++) {
            bitmap[i] = 0x00FFFFFF; // White but zero opacity
        }

        // Create texture object
        glGenTextures(1, &textureId);
        bindTexture(textureId);

        // Initilaize the empty texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    FontAtlas::~FontAtlas() {
        delete[] bitmap;
        glDeleteTextures(1, &textureId);
    }

    GLuint FontAtlas::getTextureID() const {
        return textureId;
    }

    bool FontAtlas::addGlyph(const Sizei& size, uint8_t* data, GlyphCords& coords) {
        // Deal with missing space
        bool notEnoughWidth = (texSize - cursor.x < size.x);
        if (notEnoughWidth) {
            // If there's no space left at all, give up
            if (texSize - cursor.y < size.y) { return false; }

            // If there's not enough space on a new line, give up as well
            else if (notEnoughWidth && texSize - skyline < size.y) { return false; }

            // Then all we need to do is go to the next line
            cursor = Vec2i(0, skyline);
        }

        // Blit to the bitmap
        uint8_t* bm = ((uint8_t*)&bitmap[cursor.y*texSize + cursor.x]) + 3;
        
        for (int i = 0; i < size.y; i++) {
            for (int j = 0; j < size.x; j++) {
                *bm = *(data++);
                bm += 4;
            }
            bm += 4*(texSize - size.x);
        }

        // Mark texture as no longer up to date
        textureUpToDate = false;

        // Save area of glyph
        float ratio = 1.0f / (float)texSize;
        Vec2i a = cursor;
        Vec2i b(cursor.x + size.x, cursor.y + size.y);
        coords.TL = Vec2f(a.x, a.y) * ratio;
        coords.TR = Vec2f(b.x, a.y) * ratio;
        coords.BL = Vec2f(a.x, b.y) * ratio;
        coords.BR = Vec2f(b.x, b.y) * ratio;

        // Update the skyline
        if (cursor.y + size.y > skyline) {
            skyline = cursor.y + size.y;
        }

        // Update cursor
        cursor.x += size.x;

        // Return successfully
        return true;
    }

    void FontAtlas::pushTexture() {
        // Don't do anything if the texture is already up to date
        if (textureUpToDate) { return; }
        
        // Push the texture to the GPU
        bindTexture(textureId);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texSize, texSize, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);

        // Mark as up-to-date
        textureUpToDate = true;
    }
}