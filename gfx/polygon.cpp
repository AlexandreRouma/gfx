#include "polygon.h"

namespace gfx {
    Polygon::Polygon(const std::vector<Point>& vertices) {
        // Save vertices
        this->vertices = vertices;

        // Create a list of the indices of the vertices
        std::vector<int> vertInds;
        vertInds.reserve(vertices.size());
        for (int i = 0; i < vertices.size(); i++) { vertInds.push_back(i); }

        // Triangulate the polygon
        triangulate(vertices, vertInds);
    }

    const std::vector<Point>& Polygon::getVertices() const {
        return vertices;
    }

    const std::vector<Vec3i> Polygon::getTriangles() const {
        return triangles;
    }

    void Polygon::triangulate(const std::vector<Point>& verts, const std::vector<int>& vertInds) {
        // Find spikes in the remaining vertices
        std::vector<Point> unusedVertices;
        std::vector<int> unusedVertInds;
        int vertCount = (int)verts.size();
        for (int i = 0; i < vertCount; i++) {
            // Save the vertex
            unusedVertices.push_back(verts[i]);
            unusedVertInds.push_back(vertInds[i]);

            // If this vertex is the last one, add to unused and stop
            if (i == vertCount - 1) { break; }

            // Get points
            int ib = i+1;
            int ic = (i+2)%vertCount;
            Point a = verts[i];
            Point b = verts[ib];
            Point c = verts[ic];

            // Get the angle between the vectors
            Vec2f va = c - a;
            Vec2f vb = b - a;
            float z = va.x*vb.y - va.y*vb.x;

            // If the center point is a tip, skip to the point after it and create a triangle
            if (z <= 0.0f) {
                // Check if the triangle contains a point
                bool found = false;
                // TODO

                // If the triangle contains no point, it can be saved
                if (!found) {
                    triangles.push_back(Vec3i(vertInds[i], vertInds[ib], vertInds[ic]));
                    i++;
                }
            }
        }

        // If there are still enough unused vertices to form a triangle, continue triangulating
        if (unusedVertices.size() >= 3) { triangulate(unusedVertices, unusedVertInds); }
    }
}