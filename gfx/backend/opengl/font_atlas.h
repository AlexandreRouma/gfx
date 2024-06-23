#pragma once
#include "../vendor/glad/glad.h"
#include "../../vec2.h"
#include <functional>

namespace gfx::OpenGL {
    struct GlyphCords {
        /**
         * Top-Left texture coordinate.
        */
        Vec2f TL;

        /**
         * Top-Right texture coordinate.
        */
        Vec2f TR;

        /**
         * Bottom-Left texture coordinate.
        */
        Vec2f BL;

        /**
         * Bottom-Right texture coordinate.
        */
        Vec2f BR;
    };

    class FontAtlas {
    public:
        /**
         * Create an atlas.
         * @param bindTexture Function that will be called to change the currently active OpenGL texture.
        */
        FontAtlas(const std::function<void(int)>& bindTexture);

        ~FontAtlas();

        /**
         * Get the OpenGL texture object ID associated with this atlas.
         * @return OpenGL texture ID.
        */
        GLuint getTextureID() const;

        /**
         * Get the size of the atlas's texture.
         * @return Size of the texutre in pixels.
        */
        int getTextureSize() const { return texSize; }

        /**
         * Add a glyph to the atlas.
         * @param size Size of the glyph bitmap in pixels.
         * @param data Bitmap data of the glyph in 8bit per pixel format.
         * @param position Outputs the position that the glyph was added to in the atlas.
        */
        bool addGlyph(const Size& size, uint8_t* data, GlyphCords& position);

        /**
         * Push the texture data to the GPU.
        */
        void pushTexture();

    private:
        int texSize;
        GLuint textureId;

        Point cursor;
        int skyline = 0;

        std::function<void(int)> bindTexture;

        uint32_t* bitmap;
        bool textureUpToDate = false;
    };
}