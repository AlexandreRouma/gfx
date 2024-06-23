#pragma once

namespace gfx::OpenGL {
    /**
     * Vertex shader source code.
    */
    const char* VERTEX_SHADER_SRC =
        "#version 120\n"
        "uniform mat4 projMatUnif;\n"
        "uniform vec2 posUnif;\n"
        "attribute vec2 posAttr;\n"
        "attribute vec4 colorAttr;\n"
        "attribute vec2 texCoordAttr;\n"
        "varying vec4 color;\n"
        "varying vec2 texCoord;\n"
        "void main() {\n"
        "    gl_Position = projMatUnif * vec4((posAttr + posUnif), 1.0, 1.0);\n"
        "    color = colorAttr;\n"
        "    texCoord = texCoordAttr;\n"
        "}"
    ;

    /**
     * Fragment shader source code.
    */
    const char* FRAGMENT_SHADER_SRC =
        "#version 120\n"
        "uniform sampler2D sampler;\n"
        "varying vec4 color;\n"
        "varying vec2 texCoord;\n"
        "void main() {\n"
        "    gl_FragColor = texture2D(sampler, texCoord) * color;\n"
        "}"
    ;
}