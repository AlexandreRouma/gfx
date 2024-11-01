#pragma once
#include <string>
#include <any>

namespace gfx {
    class Font {
    public:
        /**
         * Create a font from name and size. (TODO: Check if size can be a float)
        */
        Font(const std::string& name, int size);

        /**
         * Get the name of the font.
         * @return Name of the font.
        */
        std::string getName() const;

        /**
         * Set the font by name.
         * @param name Name of the font.
        */
        void setName(const std::string& name);

        /**
         * Get the size of the font.
         * @return Size of the font.
        */
        int getSize() const;

        /**
         * Set the font size.
         * @param size Size in pixels.
        */
        void setSize(int size);

        bool operator==(const Font& b) const {
            return size == b.size && name == b.name;
        }

        /**
         * For use only by the backend code.
        */
        // TODO: Get rid of this shit. Doing per drawText call is fine... Terminals will have to be done some other way then tons of drawText calls
        std::any backendTag;

    private:
        std::string name;
        int size;

    };
}

template<>
struct std::hash<gfx::Font>
{
    std::size_t operator()(const gfx::Font& s) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(s.getName());
        return h1 ^ (s.getSize() << 1);
    }
};