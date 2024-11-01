#pragma once
#include "../lia/dense/static.h"

namespace gfx {
    using Vec2f = lia::Vec2f;
    using Vec2i = lia::Vec2i;
    using Vec3i = lia::Vec3i;

    template <typename DT>
    using PointT = lia::SVec<2, DT>;
    using Point = PointT<float>;
    using Pointi = PointT<int>;
    
    template <typename DT>
    class SizeT : public lia::SVec<2, DT> {
    public:
        constexpr inline SizeT() {}

        constexpr inline SizeT(DT x, DT y) : lia::SVec<2, DT>(x, y) {}

        inline bool valid() const { return  lia::SVec<2, DT>::x > 0 && lia::SVec<2, DT>::y > 0; }
    };

    using Size = SizeT<float>;
    using Sizei = SizeT<int>;

    template <typename T>
    class RectT {
    public:
        constexpr inline RectT() {}

        constexpr inline RectT(const PointT<T>& a, const PointT<T>& b) {
            this->a = PointT<T>(std::min<T>(a.x, b.x), std::min<T>(a.y, b.y));
            this->b = PointT<T>(std::max<T>(a.x, b.x), std::max<T>(a.y, b.y));
            this->_size.x = this->b.x - this->a.x + 1;
            this->_size.y = this->b.y - this->a.y + 1;
        }

        constexpr inline RectT(const PointT<T>& a, const SizeT<T>& size) {
            this->a = a;
            this->b = a + size - PointT<T>(1, 1);
            _size = size;
        }

        constexpr inline bool operator&&(const RectT<T>& o) const {
            return !(o.b.x < a.x || o.a.x > b.x || o.b.y < a.y || o.a.y > b.y);
        }

        constexpr inline bool operator&&(const PointT<T>& o) const {
            return o.x >= a.x && o.y >= a.y && o.x <= b.x && o.y <= b.y;
        }

        // TODO: Make constexpr
        inline RectT<T> operator&(const RectT<T>& o) const {
            return RectT<T>(
                PointT<T>(std::max<T>(a.x, o.a.x), std::max<T>(a.y, o.a.y)),
                PointT<T>(std::min<T>(b.x, o.b.x), std::min<T>(b.y, o.b.y))
            );
        }

        inline const PointT<T>& A() const { return a; }
        inline const PointT<T>& B() const { return b; }
        inline const SizeT<T>& size() const { return _size; }

        constexpr inline bool contains(const PointT<T>& pos) const {
            return a.x <= pos.x && pos.x <= b.x && a.y <= pos.y && pos.y <= b.y;
        }

    private:
        PointT<T> a;
        PointT<T> b;
        SizeT<T> _size;
    };

    using Rect = RectT<float>;
    using Recti = RectT<int>;
}