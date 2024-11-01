#pragma once
#include <vector>
#include "types.h"

namespace gfx {
    class Polygon {
    public:
        /**
         * Create a polygon from a list of vertices.
         * @param vertices List of vertices in a (0,0 ; 1,1) area in a clockwise order.
        */
        Polygon(const std::vector<Point>& vertices);

        /**
         * Get the list of vertices of the polygon.
         * @return List of vertices of the polygon.
        */
        const std::vector<Point>& getVertices() const;

        /**
         * Get the list of triangles of the polygon.
         * @return List of triangles of the polygon.
        */
        const std::vector<Vec3i> getTriangles() const;
    
    private:
        void triangulate(const std::vector<Point>& verts, const std::vector<int>& vertInds);

        std::vector<Point> vertices;
        std::vector<Vec3i> triangles;
    };
}