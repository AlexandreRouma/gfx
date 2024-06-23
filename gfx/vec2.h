#pragma once
#include <math.h>
#include <algorithm>

// TODO: REDO ALL OF THIS, THE CODE QUALITY IS SHIT
// ALSO NEEDS TO BE COMMENTED

namespace gfx {

    template <class T>
    class Vec2 {
    public:
        Vec2() {}
        Vec2(T x, T y) {
            this->x = x;
            this->y = y;
        }

        constexpr inline bool operator==(const Vec2& b) const {
            return x == b.x && y == b.y;
        }

        constexpr inline bool operator!=(const Vec2& b) const {
            return x != b.x || y != b.y;
        }

        constexpr inline Vec2 operator+(const Vec2& b) const {
            return Vec2(x+b.x, y+b.y);
        }

        constexpr inline Vec2 operator-(const Vec2& b) const {
            return Vec2(x-b.x, y-b.y);
        }

        constexpr inline Vec2 operator*(const T& b) const {
            return Vec2(x*b, y*b);
        }

        constexpr inline Vec2 operator/(const T& b) const {
            return Vec2(x/b, y/b);
        }

        T norm() const {
            return sqrt(x*x + y*y);
        }

        T x = 0;
        T y = 0;
    };

    typedef Vec2<float> Vec2f;

    typedef Vec2<int> Point;
    typedef Vec2<float> Pointf;

    class Size : public Vec2<int> {
    public:
        using Vec2::Vec2;

        inline bool valid() const { return x > 0 && y > 0; }
    
    private:
        using Vec2::norm;
    };

    class Rect {
    public:
        Rect() {}

        Rect(const Point& a, const Point& b) {
            this->a = Point(std::min<int>(a.x, b.x), std::min<int>(a.y, b.y));
            this->b = Point(std::max<int>(a.x, b.x), std::max<int>(a.y, b.y));
            this->size.x = this->b.x - this->a.x + 1;
            this->size.y = this->b.y - this->a.y + 1;
        }

        Rect(const Point& a, const Size& size) {
            this->a = a;
            this->b = a + size - Point(1, 1);
            this->size = size;
        }

        constexpr inline bool operator&&(const Rect& o) const {
            return !(o.b.x < a.x || o.a.x > b.x || o.b.y < a.y || o.a.y > b.y);
        }

        constexpr inline bool operator&&(const Point& o) const {
            return o.x >= a.x && o.y >= a.y && o.x <= b.x && o.y <= b.y;
        }

        // TODO: Make constexpr
        inline Rect operator&(const Rect& o) const {
            return Rect(
                Point(std::max<int>(a.x, o.a.x), std::max<int>(a.y, o.a.y)),
                Point(std::min<int>(b.x, o.b.x), std::min<int>(b.y, o.b.y))
            );
        }

        inline const Point& getA() const { return a; }
        inline const Point& getB() const { return b; }
        inline const Size& getSize() const { return size; }

        constexpr inline bool contains(const Point& pos) const {
            return a.x <= pos.x && pos.x <= b.x && a.y <= pos.y && pos.y <= b.y;
        }

    private:
        Point a;
        Point b;
        Size size;
    };
}
