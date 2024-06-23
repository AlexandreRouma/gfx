#pragma once
#include <vector>
#include "vec2.h"

namespace gfx {
    struct TriIndices {
        int a, b, c;
    };

    class Polygon {
    public:
        /**
         * Create a polygon from a list of vertices.
         * @param vertices List of vertices in a (0,0 ; 1,1) area in a clockwise order.
        */
        Polygon(const std::vector<Pointf>& vertices);

        /**
         * Get the list of vertices of the polygon.
         * @return List of vertices of the polygon.
        */
        const std::vector<Pointf>& getVertices() const;

        /**
         * Get the list of triangles of the polygon.
         * @return List of triangles of the polygon.
        */
        const std::vector<TriIndices> getTriangles() const;
    
    private:
        void triangulate(const std::vector<Pointf>& verts, const std::vector<int>& vertInds);

        std::vector<Pointf> vertices;
        std::vector<TriIndices> triangles;
    };
}