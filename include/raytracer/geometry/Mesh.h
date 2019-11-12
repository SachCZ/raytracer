#ifndef RAYTRACER_MESH_H
#define RAYTRACER_MESH_H

#include "Triangle.h"
#include <algorithm>
#include "raytracer/utility/AdjacencyList.h"
#include "raytracer/utility/JsonFormatter.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>

namespace raytracer {
    namespace geometry {
        class Mesh {
        public:
            explicit Mesh(const std::string& filename);
            explicit Mesh(std::vector<Triangle>  triangles);

            std::vector<Triangle> getBoundary() const;
            std::vector<Triangle> getAdjacent(const Triangle& triangle) const;
            size_t getFacesCount();
            void saveToJson(const std::string& filename) const;

            const Triangle& operator[](int index) const;

        private:
            std::vector<Triangle> triangles;
            std::vector<Edge*> edges;
            utility::AdjacencyList adjacencyList;
            std::vector<size_t> boundaryEdges;

            void annotateTriangles();
            void generateAdjacencyList();
            bool isAdjacent(const Triangle& triangleA, const Triangle& triangleB) const;

            bool str_contains(const std::string& text, const std::string& toFind) const;
            Point pointFromString(const std::string& pointRepresentation) const;
            std::vector<Triangle> parseSTL(const std::string& filename) const;

            std::vector<Point> getAllPoints() const;

            std::vector<std::set<size_t>> getTrianglesAsIndexes() const;

            bool isOnBoundary(const Triangle &triangle) const;

            void annotateEdges();

            std::vector<Edge*> getAllEdges();

            std::vector<size_t> getBoundaryEdgesIndexes();

            template<typename Function>
            void iterateTriangleEdges(Function function);

            template<typename Type>
            std::vector<Type> findUniques(std::vector<Type> array);
        };
    }
}


#endif //RAYTRACER_MESH_H
