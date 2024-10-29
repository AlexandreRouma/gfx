#include "painter.h"
#include "shader.h"
#include "shader_source.h"
#include "glm/gtc/type_ptr.hpp"
#include "font_cache.h"
#include <math.h>
#include <stdexcept>

#define FL_M_PI 3.141592653589793238462643383279502884197f

#define NULL_TEXTURE    0

namespace gfx::OpenGL {
    Painter::Painter(const Size& canvasSize) {
        // Set canvas size which also generates the projection matrix
        setCanvasSize(canvasSize);

        // Load shader
        shader = std::make_shared<Shader>(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC);
        projMatUnif = shader->getUniform("projMatUnif");
        posUnif = shader->getUniform("posUnif");
        samplerUnif = shader->getUniform("sampler");
        posAttr = shader->getAttribute("posAttr");
        colorAttr = shader->getAttribute("colorAttr");
        texCoordAttr = shader->getAttribute("texCoordAttr");

        // Allocate buffer objects
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // Define vertex buffer format
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glVertexAttribPointer(posAttr, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
        glVertexAttribPointer(colorAttr, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
        glVertexAttribPointer(texCoordAttr, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(posAttr);
        glEnableVertexAttribArray(colorAttr);
        glEnableVertexAttribArray(texCoordAttr);

        // Load null texture
        uint32_t white = 0xFFFFFFFF;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, NULL_TEXTURE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        activeTexture = NULL_TEXTURE;

        // Init the font cache
        fc = new FontCache([=](int id) { selectTexture(id); });
    }

    Painter::~Painter() {
        // TODO
    }

    Size Painter::getCanvasSize() const {
        return canvasSize;
    }

    void Painter::setCanvasSize(const Size& canvasSize) {
        // Update size
        this->canvasSize = canvasSize;

        // Regenerate the projection matrix
        genProjMatrix();

        // Update the stencil
        stencil = Rect(Point(0, 0), Point(canvasSize.x - 1, canvasSize.y - 1));
        updateStencil();
    }

    void Painter::beginRender() {
        // Reset the stencil and offset
        if (!stencils.empty()) { stencils = std::stack<Rect>(); }
        if (!offsets.empty()) { offsets = std::stack<Point>(); }
        stencil = Rect(Point(0, 0), Point(canvasSize.x - 1, canvasSize.y - 1));
        offset = Point(0, 0);

        // Setup OpenGL options
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_SCISSOR_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Bind buffers
        glBindVertexArray(VAO);

        // Load shader and set uniform variables
        shader->use();
        glUniformMatrix4fv(projMatUnif, 1, GL_FALSE, glm::value_ptr(projMat));
        glUniform1i(samplerUnif, 0);

        // Set the stencil and offset
        updateStencil();
        updateOffset();

        // Configure textures to allow non-multiple of 4 widths
        // TODO: GET RID IF THIS WHEN FULL COLOR IS USED FOR FONTS
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Load texture
        glActiveTexture(GL_TEXTURE0);
        selectTexture(NULL_TEXTURE);
    }

    void Painter::endRender() {
        // Flush remaining geometry
        flush();
    }

    void Painter::pushStencil(const Rect& stencil) {
        // Flush any remaining draw commands
        flush();

        // Push the current stencil
        stencils.push(this->stencil);

        // Compute the new stencil
        Rect absStencil(stencil.getA() + offset, stencil.getB() + offset);
        Rect newStencil = this->stencil & absStencil;

        // Update the stencil
        this->stencil = newStencil;
        updateStencil();
    }

    void Painter::popStencil() {
        // Flush any remaining draw commands
        flush();

        // If no stencil was previous pushed, give up
        if (stencils.empty()) { throw std::runtime_error("Cannot pop stencil, no stencil was pushed"); }

        // Pop the stencil
        stencil = stencils.top();
        stencils.pop();

        // Update the stencil GPU-size
        updateStencil();
    }

    void Painter::pushOffset(const Point& offset) {
        // Flush any remaining draw commands
        flush();

        // Push the current offset
        offsets.push(this->offset);

        // Update the offset
        this->offset = this->offset + offset;
        updateOffset();
    }

    void Painter::popOffset() {
        // Flush any remaining draw commands
        flush();

        // If no stencil was previous pushed, give up
        if (offsets.empty()) { throw std::runtime_error("Cannot pop offset, no offset was pushed"); }

        // Pop the stencil
        offset = offsets.top();
        offsets.pop();

        // Update the offset GPU-size
        updateOffset();
    }

    void Painter::drawLine(const Point& a, const Point& b, const Color& color, int thickness) {
        // Select the null texutre
        selectTexture(NULL_TEXTURE);
        
        // Convert coordinates to floating point
        Vec2f af = Vec2f(a.x, a.y);
        Vec2f bf = Vec2f(b.x, b.y);

        // Compute forward vector
        Vec2f forw = bf - af;
        forw = forw * 0.5f / forw.norm();

        // Compute normal vector
        Vec2f norm(forw.y, -forw.x);
        norm = norm * (float)thickness;

        // Create vertices
        int tl = addVertex(af - forw - norm, color);
        int tr = addVertex(bf + forw - norm, color);
        int bl = addVertex(af - forw + norm, color);
        int br = addVertex(bf + forw + norm, color);

        // Create triangles
        addTri(tl, tr, bl);
        addTri(tr, bl, br);
    }

    void Painter::drawRect(const Rect& area, const Color& color, int thickness, int borderRadius) {
        // Select the null texutre
        selectTexture(NULL_TEXTURE);
        
        // The triangulation depends on whether the border is rounded
        if (borderRadius) {
            // TODO
        }
        else {
            // Create vertices
            float w = thickness - 1;
            int otl = addVertex(Vec2f(area.getA().x, area.getA().y) + Vec2f(-0.5f, -0.5f), color);
            int otr = addVertex(Vec2f(area.getB().x, area.getA().y) + Vec2f(0.5f, -0.5f), color);
            int obl = addVertex(Vec2f(area.getA().x, area.getB().y) + Vec2f(-0.5f, 0.5f), color);
            int obr = addVertex(Vec2f(area.getB().x, area.getB().y) + Vec2f(0.5f, 0.5f), color);
            int itl = addVertex(Vec2f(area.getA().x, area.getA().y) + Vec2f(0.5f + w, 0.5f + w), color);
            int itr = addVertex(Vec2f(area.getB().x, area.getA().y) + Vec2f(-0.5f - w, 0.5f + w), color);
            int ibl = addVertex(Vec2f(area.getA().x, area.getB().y) + Vec2f(0.5f + w, -0.5f - w), color);
            int ibr = addVertex(Vec2f(area.getB().x, area.getB().y) + Vec2f(-0.5f - w, -0.5f - w), color);

            // Create triangles
            addTri(otl, otr, itl); // Top
            addTri(otr, itl, itr);
            addTri(otl, itl, obl); // Left
            addTri(itl, obl, ibl);
            addTri(ibl, ibr, obl); // Bottom
            addTri(ibr, obl, obr);
            addTri(itr, otr, ibr); // Right
            addTri(otr, ibr, obr);
        }
    }

    void Painter::fillRect(const Rect& area, const Color& color, int borderRadius) {
        // Select the null texutre
        selectTexture(NULL_TEXTURE);
        
        // The triangulation depends on whether the border is rounded
        if (borderRadius) {
            // TODO
        }
        else {
            // Create vertices
            int tl = addVertex(Vec2f(area.getA().x, area.getA().y) + Vec2f(-0.5f, -0.5f), color);
            int tr = addVertex(Vec2f(area.getB().x, area.getA().y) + Vec2f(0.5f, -0.5f), color);
            int bl = addVertex(Vec2f(area.getA().x, area.getB().y) + Vec2f(-0.5f, 0.5f), color);
            int br = addVertex(Vec2f(area.getB().x, area.getB().y) + Vec2f(0.5f, 0.5f), color);

            // Create triangles
            addTri(tl, tr, bl);
            addTri(tr, bl, br);
        }
    }

    void Painter::drawPolygon(const Point& position, const Polygon& polygon, const Size& size, const Color& color, int thickness) {
        // TODO
    }

    void Painter::fillPolygon(const Point& position, const Polygon& polygon, const Size& size, const Color& color) {
        // Select the null texutre
        selectTexture(NULL_TEXTURE);

        // Create vertices
        int first = vertices.size();
        for (const auto& v : polygon.getVertices()) {
            addVertex(Vec2f(position.x + v.x*size.x - 0.5f, position.y + v.y*size.y - 0.5f), color);
        }

        // Create triangles
        for (const auto& t : polygon.getTriangles()) {
            addTri(first + t.a, first + t.b, first + t.c);
        }
    }

    void Painter::drawArc(const Point& center, int diameter, float startAngle, float endAngle, const Color& color, int thickness) {
        // Select the null texutre
        selectTexture(NULL_TEXTURE);
        
        // Compute external and internal radii
        float re = (float)diameter / 2.0f;
        float ri = re - (float)thickness;

        // Compute angle interval
        int vcount = ceil(fabsf((endAngle - startAngle) * re));
        float dtheta = (endAngle - startAngle) / vcount;
        vcount++;

        // Convert center point to float
        Vec2f cf = Vec2f(center.x, center.y);

        // Create vertices
        float theta = startAngle;
        for (int i = 0; i < vcount; i++) {
            Vec2f phase(cosf(theta), sinf(theta));
            addVertex(cf - phase*re, color);
            addVertex(cf - phase*ri, color);
            theta += dtheta;
        }

        // Create triangles
        int last = vertices.size() - 2;
        int first = vertices.size() - vcount*2;
        for (int i = first; i < last; i += 2) {
            addTri(i, i+1, i+2);
            addTri(i+1, i+2, i+3);
        }
    }

    void Painter::fillArc(const Point& center, int diameter, float startAngle, float endAngle, const Color& color) {
        // Select the null texutre
        selectTexture(NULL_TEXTURE);

        // Compute radius
        float re = (float)diameter / 2.0f;

        // Compute angle interval
        int vcount = ceil(fabsf((endAngle - startAngle) * re));
        float dtheta = (endAngle - startAngle) / vcount;
        vcount++;

        // Create center vertex
        Vec2f cf = Vec2f(center.x, center.y);
        int c = addVertex(Vec2f(center.x, center.y), color);

        // Create vertices
        float theta = startAngle;
        for (int i = 0; i < vcount; i++) {
            addVertex(cf - Vec2f(cosf(theta)*re, sinf(theta)*re), color);
            theta += dtheta;
        }

        // Create triangles
        int last = vertices.size() - 1;
        int first = vertices.size() - vcount;
        for (int i = first; i < last; i++) {
            addTri(i, i+1, c);
        }
    }

    inline int getCodepoint(const char*& str) {
        int len = 0;
        int id = 0;
        for (unsigned char c; c = *str, c; str++) {
            // If null, this is the end of the string, give up
            if (!c) { break; }

            // If a start byte, set beginning of ID and length
            if ((c >> 7) == 0) {
                id = c;
                break;
            }
            else if ((c >> 5) == 0b110) {
                id = (int)(c & 0b11111) << 6;
                len = 1;
            }
            else if ((c >> 4) == 0b1110) {
                id = (int)(c & 0b1111) << 6;
                len = 2;
            }
            else if ((c >> 3) == 0b11110) {
                id = (int)(c & 0b111) << 6;
                len = 3;
            }
            
            // If a continuation byte, 
            else if ((c >> 6) == 0b10) {
                // If there's nothing to read, ignore this byte
                if (!len) { continue; }

                // Add bits to the ID
                id |= c & 0b111111;

                // If there are remaining continuation bytes, shift bits up
                if (--len) { id <<= 6; }
            }

            // If we're done reading, break out
            if (!len) { break; }
        }
        str++;
        return id;
    }

    Size Painter::measureText(Font& font, const char* str) {
        // Begin the cursor at 0
        double cursor = 0.0;

        // Prepare the font
        fc->prepareFont(font);

        // Iterate over all characters
        while (true) {
            // Get unicode ID
            int id = getCodepoint(str);
            if (!id) { break; }

            // Compute the sub-pixel alignment
            int x = floorf(cursor);
            int subx = floorf((cursor - x) * 4.0f);

            // Fetch glyph info
            GlyphInfo info = fc->getGlyph(font, id, subx);

            // Update cursor
            cursor += info.xAdvance;
        }

        return Size(ceil(cursor), font.getSize());
    }

    void Painter::drawText(const Point& position, const char* str, Font& font, const Color& color, HRef href, VRef vref) {
        // Initialize cursor
        Vec2f cursor(position.x, position.y);

        // Do horizontal alignment
        if (href == H_REF_LEFT) {
            // Prepare the font since measureText (which would prepare it) isn't called
            fc->prepareFont(font);
        }
        else {
            // Get the horizontal measurements of the text
            Size tsize = measureText(font, str);

            // Move cursor depending on the desired alignement
            if (href == H_REF_CENTER) {
                cursor.x -= tsize.x / 2;
            }
            else if (href == H_REF_RIGHT) {
                cursor.x -= tsize.x;
            }
        }

        // Do vertical alignement
        if (vref != V_REF_BASELINE) {
            // Get the font metrics
            FontMetrics metrics = fc->getFontMetrics(font);

            // Move the cursor depending on the desired alignement
            if (vref == V_REF_BOTTOM) {
                cursor.y += metrics.descender;
            }
            else if (vref == V_REF_CENTER) {
                cursor.y += (metrics.descender + metrics.ascender) * 0.5f;
            }
            else if (vref == V_REF_TOP) {
                cursor.y += metrics.ascender;
            }

            // Quantize the position (TODO: Change this if fractional y position support is added)
            cursor.y = round(cursor.y);
        }
        
        // Iterate over all characters
        while (true) {
            // Get unicode ID
            int id = getCodepoint(str);
            if (!id) { break; }

            // TODO: Optimise away spaces

            // Compute the sub-pixel alignment
            int x = floorf(cursor.x);
            int subx = floorf((cursor.x - x) * 4.0f);

            // Fetch glyph info
            GlyphInfo info = fc->getGlyph(font, id, subx);

            // Select the texture of the atlas
            selectTexture(info.textureId);

            // Create vertices
            Vec2f tlp = Vec2f(x + info.offset.x - 0.5f, cursor.y - info.offset.y - 0.5f);
            int tl = addVertex(tlp, color, info.coords.TL);
            int tr = addVertex(tlp + Vec2f(info.size.x, 0), color, info.coords.TR);
            int bl = addVertex(tlp + Vec2f(0, info.size.y), color, info.coords.BL);
            int br = addVertex(tlp + Vec2f(info.size.x, info.size.y), color, info.coords.BR);

            // Create triangles
            addTri(tl, tr, bl);
            addTri(tr, bl, br);

            // TODO: Kerning

            // Update cursor
            cursor.x += info.xAdvance;
        }
    }

    void Painter::genProjMatrix() {
        // Create matrix to convert from (0,0 -> w,h) to (-1,1 -> 1,-1)
        projMat = glm::ortho(-0.5f, (float)(canvasSize.x - 1) + 0.5f, (float)(canvasSize.y - 1) + 0.5f, -0.5f);
    }

    int Painter::addVertex(const Vec2f& pos, const Color& color, const Vec2f& texCoord) {
        VertexAttrib vert;
        vert.pos[0] = pos.x;
        vert.pos[1] = pos.y;
        vert.color[0] = color.r;
        vert.color[1] = color.g;
        vert.color[2] = color.b;
        vert.color[3] = color.a;
        vert.texCoord[0] = texCoord.x;
        vert.texCoord[1] = texCoord.y;
        vertices.push_back(vert);
        return vertices.size() - 1;
    }

    void Painter::addTri(int a, int b, int c) {
        indices.push_back(a);
        indices.push_back(b);
        indices.push_back(c);
    }

    void Painter::flush() {
        // If there's nothing to draw, flush vertices and return
        if (indices.empty()) {
            vertices.clear();
            return;
        }

        // Replace or reallocate vertex/index buffers with new data
        int vertCount = vertices.size();
        
        // Load vertex data
        if (vertCount > VBOCapacity) {
            // Buffer must be reallocated since it's too small
            glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(VertexAttrib), vertices.data(), GL_DYNAMIC_DRAW);
            VBOCapacity = vertCount;
        }
        else {
            // Data can simply be substituted
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertCount * sizeof(VertexAttrib), vertices.data());
        }

        // Load index data
        int indCount = indices.size();
        if (indCount > EBOCapacity) {
            // Buffer must be reallocated since it's too small
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indCount * sizeof(int), indices.data(), GL_DYNAMIC_DRAW);
            EBOCapacity = indCount;
        }
        else {
            // Data can simply be substituted
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indCount * sizeof(int), indices.data());
        }

        // Draw triangles using indices
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

        // Flush buffers
        vertices.clear();
        indices.clear();
    }

    void Painter::updateStencil() {
        glScissor(stencil.getA().x, canvasSize.y - stencil.getB().y - 1, stencil.getSize().x, stencil.getSize().y);
    }

    void Painter::updateOffset() {
        glUniform2f(posUnif, offset.x, offset.y);
    }

    void Painter::selectTexture(GLuint id) {
        // If the texture is already active, do nothing
        if (id == activeTexture) { return; }

        // Flush all current triangles
        flush();

        // Update selected texture
        glBindTexture(GL_TEXTURE_2D, id);
        activeTexture = id;
    }
}