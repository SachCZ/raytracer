#ifndef RAYTRACER_GEOMETRY_PRIMITIVES_H
#define RAYTRACER_GEOMETRY_PRIMITIVES_H

#include <vector>
#include <ostream>

/**
 * Namespace of the whole library.
 */
namespace raytracer {

    /**
     * \addtogroup api
     * @{
     */

    /**
     * Class representing a point
     * A point is given by two coordinates x and y
     */
    class Point {
    public:
        /**
         * Construct the edge given x and y.
         * Note that x goes first, be sure not to mix this up.
         * @param x coordinate
         * @param y coordinate
         */
        Point(double x, double y);

        /**
         * Default constructor.
         * Default constructed point has both x and y equal to 0
         */
        Point() = default;

        /**
         * x coordinate
         */
        double x;
        /**
         * y coordinate
         */
        double y;
    };


    /**
     * Class representing a physical vector
     */
    class Vector {
    public:
        /**
         * Construct the vector using its coordinates.
         * @param x coordinate
         * @param y coordinate
         */
        Vector(double x, double y);

        /**
         * Default constructor initializes a (0, 0) vector.
         */
        Vector() = default;

        /**
         * x coordinate
         */
        double x;
        /**
         * y coordinate
         */
        double y;

        /**
         * Return the Euclidean norm of the vector (square root of sum of coordinates squared)
         * @return size of the vector
         */
        double getNorm() const;

        /**
         * Return the normal to the vector using the convention (y, -x)
         * @return
         */
        Vector getNormal() const {
            return {this->y, -this->x};
        }
    };

    /**
     * Two Points can be subtracted to get a Vector
     * @param A point
     * @param B point
     * @return vector given by points difference
     */
    const Vector operator-(Point A, Point B);

    /**
     * Vector and point can be added get another point.
     * @param A point
     * @param b vector
     * @return Point given by vector added to point A
     */
    const Point operator+(Point A, Vector b);

    /**
     * Convenience wrapper around A + b
     * @param b
     * @param A
     * @return
     */
    const Point operator+(Vector b, Point A);

    std::ostream& operator<<(std::ostream& os, const Point& point);

    /**
     * Vector can be multiplied by number k*a
     * @param k
     * @param a
     * @return scaled vector
     */
    const Vector operator*(double k, Vector a);

    /**
     * Convenience wrapper around k*a
     * @param a
     * @param k
     * @return
     */
    const Vector operator*(Vector a, double k);

    /**
     * his is the dot product of two vectors a, b
     * @param a
     * @param b
     * @return dot product
     */
    double operator*(Vector a, Vector b);

    /**
     * Two vectors a, b can be added
     * @param a
     * @param b
     * @return
     */
    const Vector operator+(Vector a, Vector b);

    /**
     * Two vectors a, b can be subtracted
     * @param a
     * @param b
     * @return
     */
    const Vector operator-(Vector a, Vector b);

    std::ostream &operator<<(std::ostream &os, const Vector &vector);


    /**
     * Class representing a polygonal face in mesh (edge in 2D, surface polygon face in 3D).
     * @warning
     * Do not construct this manually unless you know what you are doing.
     */
    class Face {
    public:
        /**
         *  Calculate a normal to the face (edge in 2D).
         *  By convention in 2D, the normal is outward for points
         *  in clockwise order forming a polygon.
         *  @return the normal vector.
         */
        Vector getNormal() const;

        /**
         * Get the points forming the face.
         *
         * @return the points.
         */
        const std::vector<Point *> &getPoints() const;

        /**
         * Construct the face using an id and std::vector of points that the face consist of.
         *
         * @warning
         * It is users responsibility for the id to be unique. It is thus advised to not construct instance of Face
         * manually.
         *
         * @param id unique identification
         * @param points the face consists of
         */
        explicit Face(int id, std::vector<Point *> points);

        /**
         * Get the id of the face.
         * @return id
         */
        int getId() const;

    private:
        int id;
        std::vector<Point *> points;
    };


        /**
         * Class representing a single polygonal element in mesh. Could be 2D/3D given by set of faces (edges, surfaces).
         */
        class Element {
        public:
            /**
             * Get the faces of the Element.
             * Edges in 2D, surfaces in 3D.
             * @return the faces
             */
            const std::vector<Face *> &getFaces() const;

            /**
             * Get the points of the Element.
             * @return the points
             */
            const std::vector<Point *> &getPoints() const {
                return this->points;
            };

            /**
             * Constructor taking an id and std::vector of faces of the element.
             * @warning
             * It is users responsibility for the id to be unique. It is thus advised to not construct instance of Element
             * manually.
             *
             * Clockwise order of faces is preferred in 2D.
             * @param id unique identification
             * @param faces of the element
             */
            explicit Element(int id, std::vector<Face *> faces);

            /**
             * Retrieve the id of element.
             * @return id
             */
            int getId() const;

        private:
            int id;
            std::vector<Face *> faces;
            std::vector<Point *> points;
        };

        Point getElementCentroid(const Element &element);


    /**
     * @}
     */
}

#endif //RAYTRACER_GEOMETRY_PRIMITIVES_H
