#ifndef RAYTRACER_MESH_H
#define RAYTRACER_MESH_H

#include <vector>
#include <string>

#include "Triangle.h"
#include "raytracer/utility/AdjacencyList.h"

/**
 * Namespace of the whole library
 */
namespace raytracer {
    /**
     * Implementation namespace
     * Ignore this as a API user!
     */
    namespace impl {
        class MeshSerializer {
        public:
            std::vector<geometry::Triangle> parseSTL(const std::string &filename) const;

            void saveToJson(const geometry::Mesh &mesh, const std::string &filename) const;

        private:
            bool str_contains(const std::string &text, const std::string &toFind) const;

            geometry::Point pointFromString(const std::string &pointRepresentation) const;
        };
    }

    /**
     * Namespace encapsulating all the abstractions related to geometry
     */
    namespace geometry {

        /**
         * Class representing a triangulated mesh.
         * Mesh class represents a triangulated mesh and provides ways to query the mesh for info. It also
         * encapsulates some basic loading and storing capabilities.
         */
        class Mesh {
        public:

            /**
             * Constructs a 2D mesh given an STL file
             * Use ASCII Stereolithography (.stl) file to initialize the mesh. Such a file can be generated
             * using specialized software. It represents a 3D mesh by default. Here the z dimension is ignored.
             * This constructor throws an std::logic_error if the file loading fails.
             * @param filename name of the file to be loaded
             */
            explicit Mesh(const std::string &filename);

            /**
             * Constructs a 2D mesh given a list of triangles
             * The given triangles could be arbitrary. If some of the triangles share edges (meaning the real
             * point values of edges no C++ objects) they will be evaluated as adjacent.
             * @param triangles list of triangles
             */
            explicit Mesh(std::vector<Triangle> triangles);

            /**
             * Returns triangles on boundary.
             * Triangle is classified as being on boundary if it has two or less adjacent triangles.
             * @return list of triangles on boundary
             */
            std::vector<Triangle> getBoundary() const;

            /**
             * Returns the adjacent triangles.
             * This will work only given a triangle from the mesh (obtained for example by another getAdjacent call).
             * @param triangle whose adjacent triangles to get
             * @return list of adjacent triangles
             */
            std::vector<Triangle> getAdjacent(const Triangle &triangle) const;

            /**
             * Returns the triangle count.
             * @return number of triangles
             */
            size_t getFacesCount();

            /**
             * Saves the mesh using JSON.
             * It serialized the mesh into two main JSON objects: points and triangles
             * both being sequences. Points is a sequence of points eg. [[3,5], [-1,3]].
             * Triangles is a sequence of point indexes eg [0, 2, 3] meaning the three points making up the
             * triangle are the zeroth second and third point in the points sequence.
             * This format is useful for matplotlib visualization.
             * @param filename filename without suffix (.json will be added)
             */
            void saveToJson(const std::string &filename) const;

            /**
             * Subscript operator.
             * Use it to access the i-th triangle of the mesh
             * @param index index to access
             * @return a triangle
             */
            const Triangle &operator[](int index) const;

        private:
            std::vector<Triangle> triangles;
            utility::AdjacencyList adjacencyList;
            static impl::MeshSerializer serializer;

            void annotateTriangles();

            void generateAdjacencyList();

            bool isAdjacent(const Triangle &triangleA, const Triangle &triangleB) const;

            std::vector<Point> getAllPoints() const;

            std::vector<std::set<size_t>> getTrianglesAsIndexes() const;

            bool isOnBoundary(const Triangle &triangle) const;

            friend class impl::MeshSerializer;
        };
    }
}


#endif //RAYTRACER_MESH_H
