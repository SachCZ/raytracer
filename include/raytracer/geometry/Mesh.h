#ifndef RAYTRACER_MESH_H
#define RAYTRACER_MESH_H

#include <vector>
#include <string>
#include <algorithm>
#include <memory>

#include "Quadrilateral.h"
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
        class MeshSerializer;
    }

    /**
     * Namespace encapsulating all the abstractions related to geometry
     */
    namespace geometry {

        /**
         * Class representing a mesh of quadrilaterals.
         * Mesh class represents a mesh and provides ways to query the mesh for info. It also
         * encapsulates some basic loading and storing capabilities.
         */
        class Mesh {
        public:

            /**
             * Constructs a 2D mesh given an STL file
             * Use ".vtk" file to initialize the mesh. Such a file can be generated
             * using specialized software.
             * This constructor throws an std::logic_error if the file loading fails.
             * @param filename name of the file to be loaded
             */
            explicit Mesh(const std::string &filename);

            /**
             * Constructs a 2D mesh given a list of points and quadrilaterals as indexes in the list of points
             * If some of the quadrilaterals share edges (meaning the real
             * point values of edges no C++ objects) they will be evaluated as adjacent.
             * @param quadrilaterals list of quadrilaterals
             */
            explicit Mesh(const std::vector<Point>& points, std::vector<std::vector<size_t>>  quadIndexes);

            /**
             * Returns the adjacent quadrilaterals.
             * This will work only given a quadrilateral from the mesh (obtained for example by another getAdjacent call).
             * @param quadrilateral whose adjacent quads to get
             * @return list of adjacent quads
             */
            std::vector<Quadrilateral> getAdjacent(const Quadrilateral &quadrilateral) const;

            /**
             * Returns the quad count.
             * @return number of quads
             */
            size_t getFacesCount();

            /**
             * Saves the mesh using JSON.
             * It serialized the mesh into two main JSON objects: points and quadrilaterals
             * both being sequences. Points is a sequence of points eg. [[3,5], [-1,3]].
             * Quadrilateral is a sequence of point indexes eg [0, 2, 3, 4] meaning the four points making up the
             * quadrilateral are the zeroth second, third and fourth point in the points sequence.
             * This format is useful for matplotlib visualization.
             * @param filename filename without suffix (.json will be added)
             */
            void saveToJson(const std::string &filename) const;

            /**
             * Subscript operator.
             * Use it to access the i-th quadrilateral of the mesh
             * @param index index to access
             * @return a quad
             */
            const Quadrilateral &operator[](int index) const;

            /**
             * Get the list of all quadrilaterals (as a const reference)
             * @return list of quadrilaterals
             */
            const std::vector<Quadrilateral>& getQuads() const;

            std::vector<raytracer::geometry::Quadrilateral> boundary;

        private:
            std::vector<std::unique_ptr<Point>> points;
            std::vector<std::vector<size_t>> quadIndexes;
            std::vector<Quadrilateral> quads;
            utility::AdjacencyList adjacencyList;
            static impl::MeshSerializer serializer;

            std::vector<Quadrilateral> getBoundary() const;

            std::vector<Quadrilateral> generateQuads();

            Quadrilateral quadFromIndexes(const std::vector<size_t>& indexes);

            void generateAdjacencyList();

            bool isAdjacent(const Quadrilateral &quadA, const Quadrilateral &quadB) const;

            std::vector<Point> getPoints() const;

            std::vector<std::vector<size_t>> getQuadsAsIndexes() const;

            bool isOnBoundary(const Quadrilateral &quad) const;

            friend class impl::MeshSerializer;

            void annotateQuads();
        };
    }

    namespace impl {
        class MeshSerializer {
        public:
            geometry::Mesh parseVTK(const std::string &filename) const;

            void saveToJson(const geometry::Mesh &mesh, const std::string &filename) const;

        private:
            bool strContains(const std::string &text, const std::string &toFind) const;

            geometry::Point pointFromString(const std::string &pointRepresentation) const;

            std::vector<size_t> quadIndexesFromString(const std::string& basicString) const;
        };
    }
}


#endif //RAYTRACER_MESH_H
