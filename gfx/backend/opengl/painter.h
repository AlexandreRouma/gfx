#pragma once
#include "../../painter.h"
#include "shader.h"
#include "font_cache.h"
#include <memory>
#include <vector>
#include <stack>

namespace gfx::OpenGL {
#pragma pack(push, 1)
    struct VertexAttrib {
        float pos[2];
        float color[4];
        float texCoord[2];
    };
#pragma pack(pop)

    class Painter : public gfx::Painter {
    public:
        /**
         * Create an OpenGL-based painter. OpenGL must be loaded and available when this constructor is called.
         * @param canvasSize Size of the canvas.
        */
        Painter(const Sizei& canvasSize);

        // Destructor
        ~Painter();

        /**
         * Get the size of the canvas.
         * @return Size of the canvas.
        */
       Sizei getCanvasSize() const;

        /**
         * Set the size of the canvas.
         * @param canvasSize Size of the canvas.
        */
        void setCanvasSize(const Sizei& canvasSize);

        /**
         * Start the rendering procedure.
        */
        void beginRender();

        /**
         * Stop the rendering procedure.
        */
        void endRender();

        void pushStencil(const Recti& stencil);

        void popStencil();

        void pushOffset(const Pointi& offset);

        void popOffset();

        /**
         * Draw a line.
         * @param a Starting point.
         * @param b Ending point.
         * @param color Color of the line.
         * @param thickness Thickness of the line in pixels
        */
        void drawLine(const Point& a, const Point& b, const Color& color, float thickness = 1);

        /**
         * Draw a hollow rectangle.
         * @param area Area of rectangle including border.
         * @param color Color of the rectangle.
         * @param thickness Thickness of the border in pixels.
         * @param borderRadius Rounding radius in pixels.
        */
        void drawRect(const Rect& area, const Color& color, float thickness = 1, float borderRadius = 0);

        /**
         * Draw a filled rectangle.
         * @param area Area of rectangle including border.
         * @param color Color of the rectangle.
         * @param borderRadius Rounding radius in pixels.
        */
        void fillRect(const Rect& area, const Color& color, float borderRadius = 0);

        /**
         * Draw a hollow polygon.
         * @param position Position of the top left corner of the polygon.
         * @param polygon The polygon to draw.
         * @param size Size of the bounding box of the polygon.
         * @param color Color of the polygon.
         * @param thickness Thickness of the border in pixels.
        */
        void drawPolygon(const Point& position, const Polygon& polygon, const Size& size, const Color& color, float thickness = 1);

        /**
         * Draw a filled polygon.
         * @param position Position of the top left corner of the polygon.
         * @param polygon The polygon to draw.
         * @param size Size of the bounding box of the polygon.
         * @param color Color of the polygon.
        */
        void fillPolygon(const Point& position, const Polygon& polygon, const Size& size, const Color& color);
        
        /**
         * Draw a hollow arc. The arc is drawn in a clockwise direction with the reference point being on the left.
         * @param center Center point.
         * @param diameter Diameter of the arc in pixels.
         * @param startAngle Angle at which the arc start.
         * @param endAngle Angle at which the arc ends.
         * @param color  Color of the arc.
        */
        void drawArc(const Point& center, float diameter, float startAngle, float endAngle, const Color& color, float thickness = 1);

        /**
         * Draw a filled arc. The arc is drawn in a clockwise direction with the reference point being on the left.
         * @param center Center point.
         * @param diameter Diameter of the arc in pixels.
         * @param startAngle Angle at which the arc start.
         * @param endAngle Angle at which the arc ends.
         * @param color  Color of the arc.
        */
        void fillArc(const Point& center, float diameter, float startAngle, float endAngle, const Color& color);

        /**
         * Measure the size of a string.
         * @param str String to draw.
         * @param font Font to use to draw the string.
        */
        Size measureText(Font& font, const char* str);

        /**
         * Draw a string.
         * @param position Position at which the string will be draw.
         * @param str String to draw.
         * @param font Font to use to draw the string.
         * @param color Color of the text.
         * @param href Horizontal reference point.
         * @param vref Vertical reference point.
        */
        void drawText(const Point& position, const char* str, Font& font, const Color& color, HRef href = H_REF_LEFT, VRef vref = V_REF_BASELINE);

        FontCache* fc = NULL;

    private:
        void genProjMatrix();
        // TODO: The default texcoord should probably be 0.5f, 0.5f to make sure even linear selection gets full color
        int addVertex(const Vec2f& pos, const Color& color, const Vec2f& texCoord = Vec2f(0, 0));
        void addTri(int a, int b, int c);
        void flush();
        void selectTexture(GLuint id);
        void updateStencil();
        void updateOffset();
        // TODO: Function to load the texture

        Sizei canvasSize;
        int VBOCapacity = 0;
        int EBOCapacity = 0;
        std::stack<Recti> stencils;
        std::stack<Pointi> offsets;

        // GPU-side variables
        std::shared_ptr<Shader> shader;
        GLuint scaleUnif;
        GLuint offsetUnif;
        GLuint samplerUnif;
        GLuint posAttr;
        GLuint colorAttr;
        GLuint texCoordAttr;
        GLuint activeTexture;

        // CPU-side OpenGL variables
        Vec2f scaleVec;
        Vec2f offsetVec;
        std::vector<VertexAttrib> vertices;
        std::vector<int> indices;
        Recti stencil;
        Pointi offset;

        // OpenGL buffers objects
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
    };
}