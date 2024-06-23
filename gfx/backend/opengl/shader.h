#pragma once
#include "glad/glad.h"
#include <string>
#include <map>

namespace gfx::OpenGL {
    class Shader {
    public:
        /**
         * Create an OpenGL shader program.
         * @param vertSource Vertex shader source.
         * @param fragSource Fragment shader source.
        */
        Shader(const std::string& vertSource, const std::string& fragSource);

        ~Shader();

        /**
         * Get program ID.
        */
        operator GLint() const { return prog; }

        /**
         * Get attribute ID by name.
         * @param name Name of the attribute.
         * @return ID of the attribute.
        */
        GLuint getAttribute(const std::string& name);

        /**
         * Get uniform ID by name.
         * @param name Name of the uniform.
         * @return ID of the uniform.
        */
        GLuint getUniform(const std::string& name);
        
        /**
         * Activate shader program.
        */
        void use();
        
    private:
        static void checkShader(GLint shader);
        static void checkProgram(GLint program);

        GLint prog = -1;
        std::map<std::string, GLuint> attribs;
        std::map<std::string, GLuint> uniforms;
    };
}