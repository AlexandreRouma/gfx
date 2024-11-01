#pragma once
#include "types.h"
#include "color.h"
#include "polygon.h"
#include "font.h"
#include <string>

namespace gfx {
    /**
     * Horizontal reference.
    */
    enum HRef {
        H_REF_LEFT,
        H_REF_CENTER,
        H_REF_RIGHT
    };

    /**
     * Vertical reference.
    */
    enum VRef {
        V_REF_TOP,
        V_REF_CENTER,
        V_REF_BASELINE,
        V_REF_BOTTOM
    };

    // TODO: Switch to floats !!!!!!!!!!!!!!!!!!!!!!

    class Painter {
    public:
        // Virtual destructor
        virtual ~Painter() {}

        virtual void pushStencil(const Recti& stencil) = 0;

        virtual void popStencil() = 0;

        virtual void pushOffset(const Pointi& offset) = 0;

        virtual void popOffset() = 0;

        /**
         * Draw a line.
         * @param a Starting point.
         * @param b Ending point.
         * @param color Color of the line.
         * @param thickness Thickness of the line in pixels
        */
        virtual void drawLine(const Point& a, const Point& b, const Color& color, float thickness = 1) = 0;

        /**
         * Draw a hollow rectangle.
         * @param area Area of rectangle including border.
         * @param color Color of the rectangle.
         * @param thickness Thickness of the border in pixels.
         * @param borderRadius Rounding radius in pixels.
        */
        virtual void drawRect(const Rect& area, const Color& color, float thickness = 1, float borderRadius = 0) = 0;

        /**
         * Draw a filled rectangle.
         * @param area Area of rectangle including border.
         * @param color Color of the rectangle.
         * @param borderRadius Rounding radius in pixels.
        */
        virtual void fillRect(const Rect& area, const Color& color, float borderRadius = 0) = 0;

        /**
         * Draw a hollow polygon.
         * @param position Position of the top left corner of the polygon.
         * @param polygon The polygon to draw.
         * @param size Size of the bounding box of the polygon.
         * @param color Color of the polygon.
         * @param thickness Thickness of the border in pixels.
        */
        virtual void drawPolygon(const Point& position, const Polygon& polygon, const Size& size, const Color& color, float thickness = 1) = 0;

        /**
         * Draw a filled polygon.
         * @param position Position of the top left corner of the polygon.
         * @param polygon The polygon to draw.
         * @param size Size of the bounding box of the polygon.
         * @param color Color of the polygon.
        */
        virtual void fillPolygon(const Point& position, const Polygon& polygon, const Size& size, const Color& color) = 0;

        /**
         * Draw a hollow arc. The arc is drawn in a clockwise direction with the reference point being on the left.
         * @param center Center point.
         * @param diameter Diameter of the arc in pixels.
         * @param startAngle Angle at which the arc start.
         * @param endAngle Angle at which the arc ends.
         * @param color  Color of the arc.
        */
        virtual void drawArc(const Point& center, float diameter, float startAngle, float endAngle, const Color& color, float thickness = 1) = 0;

        /**
         * Draw a filled arc. The arc is drawn in a clockwise direction with the reference point being on the left.
         * @param center Center point.
         * @param diameter Diameter of the arc in pixels.
         * @param startAngle Angle at which the arc start.
         * @param endAngle Angle at which the arc ends.
         * @param color  Color of the arc.
        */
        virtual void fillArc(const Point& center, float diameter, float startAngle, float endAngle, const Color& color) = 0;

        /**
         * Measure the size of a string.
         * @param str String to draw.
         * @param font Font to use to draw the string.
        */
        virtual Size measureText(Font& font, const char* str) = 0;

        /**
         * Measure the size of a string.
         * @param str String to draw.
         * @param font Font to use to draw the string.
        */
        inline Size measureText(Font& font, const std::string& str) {
            return measureText(font, str.c_str());
        }

        /**
         * Draw a string.
         * @param position Position at which the string will be draw.
         * @param str String to draw.
         * @param font Font to use to draw the string.
         * @param color Color of the text.
         * @param href Horizontal reference point.
         * @param vref Vertical reference point.
        */
        virtual void drawText(const Point& position, const char* str, Font& font, const Color& color, HRef href = H_REF_LEFT, VRef vref = V_REF_BASELINE) = 0;

        /**
         * Draw a string.
         * @param position Position at which the string will be draw.
         * @param str String to draw.
         * @param font Font to use to draw the string.
         * @param color Color of the text.
         * @param href Horizontal reference point.
         * @param vref Vertical reference point.
        */
        inline void drawText(const Point& position, const std::string& str, Font& font, const Color& color, HRef href = H_REF_LEFT, VRef vref = V_REF_BASELINE) {
            drawText(position, str.c_str(), font, color, href, vref);
        };
    };
}