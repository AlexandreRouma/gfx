#include "font.h"

namespace gfx {
    Font::Font(const std::string& name, int size) {
        this->name = name;
        this->size = size;
    }

    std::string Font::getName() const {
        return name;
    }

    void Font::setName(const std::string& name) {
        // Update the name
        this->name = name;

        // Reset the backend tag
        backendTag.reset();
    }

    int Font::getSize() const {
        return size;
    }

    void Font::setSize(int size) {
        // Update the size
        this->size = size;

        // Reset the backend tag
        backendTag.reset();
    }
}