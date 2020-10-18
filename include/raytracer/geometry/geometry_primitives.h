#ifndef RAYTRACER_GEOMETRY_PRIMITIVES_H
#define RAYTRACER_GEOMETRY_PRIMITIVES_H

#include <vector>
#include <ostream>

/**
 * Namespace of the whole library.
 */
namespace raytracer {
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
         * @return the normal
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
     * Same as A + b
     * @param b
     * @param A
     * @return
     */
    const Point operator+(Vector b, Point A);

    std::ostream &operator<<(std::ostream &os, const Point &point);

    /**
     * Number times vector
     * @param k
     * @param a
     * @return scaled vector
     */
    const Vector operator*(double k, Vector a);

    /**
     * Vector times number
     * @param a
     * @param k
     * @return
     */
    const Vector operator*(Vector a, double k);

    /**
     * Dot product a*b
     * @param a
     * @param b
     * @return dot product
     */
    double operator*(Vector a, Vector b);

    /**
     * Add vectors a + b
     * @param a
     * @param b
     * @return
     */
    const Vector operator+(Vector a, Vector b);

    /**
     * Subtract vectors a - b
     * @param a
     * @param b
     * @return
     */
    const Vector operator-(Vector a, Vector b);

    std::ostream &operator<<(std::ostream &os, const Vector &vector);


    /**
     * Face is a collection of points
     */
    class Face {
    public:
        /**
         *  Calculate a normal to the face (edge in 2D).
         *  The normal is outward for points
         *  in clockwise order forming a 2D polygon.
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
         * Construct the face using an id and points.
         *
         * @param id unique identification
         * @param points
         */
        explicit Face(int id, std::vector<Point *> points);

        /**
         * Get the id
         * @return id
         */
        int getId() const;

    private:
        int id;
        std::vector<Point *> points;
    };


    /**
     * Element is a collection of faces
     */
    class Element {
    public:
        /**
         * Get the faces.
         * @return the faces
         */
        const std::vector<Face *> &getFaces() const;

        /**
         * Get the points of the faces.
         * @return the points
         */
        const std::vector<Point *> &getPoints() const {
            return this->points;
        };

        /**
         * Construct the element using an id and faces
         *
         * @param id unique identification
         * @param faces
         */
        explicit Element(int id, std::vector<Face *> faces);

        /**
         * Retrieve the id
         * @return id
         */
        int getId() const;

    private:
        int id;
        std::vector<Face *> faces;
        std::vector<Point *> points;
    };

    /**
    * Part of a line that has a fixed starting point but no end point
    */
    struct Ray {
        /**
         * Starting point
         */
        Point origin;
        /**
         * Direction
         */
        Vector direction;
    };

    /**
     * Point and a face with unique id
     */
    struct PointOnFace {
        /**
         * The point.
         */
        Point point;
        /**
         * Pointer to the face the point is at.
         */
        const Face *face;

        int id;
    };

    /**
     * Given an element, calculate and return its centroid
     * @param element
     * @return centroid
     */
    Point getElementCentroid(const Element &element);
}

#endif //RAYTRACER_GEOMETRY_PRIMITIVES_H
