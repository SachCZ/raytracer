#ifndef RAYTRACER_GEOMETRY_PRIMITIVES_H
#define RAYTRACER_GEOMETRY_PRIMITIVES_H

#include <vector>
#include <ostream>
#include <cmath>

/**
 * Namespace of the whole library.
 */
namespace raytracer {
    class Vector;

    /**
     * Class representing a point
     * A point is given by two coordinates x and y
     */
    class Point {
    public:
        /** Vector is convertible to point */
        explicit Point(const Vector &vector);

        /** Can be constructed from coords */
        Point(double x, double y, int id = 0);

        Point() = default;

        /**
         * x coordinate
         */
        double x{};
        /**
         * y coordinate
         */
        double y{};

        int id{};
    };


    /**
     * Class representing a physical vector
     */
    class Vector {
    public:
        /**
         * x coordinate
         */
        double x;
        /**
         * y coordinate
         */
        double y;

        /** Point is convertible to vector */
        explicit Vector(const Point &point);

        /** Vector can be constructed from coordinates */
        Vector(double x, double y);

        Vector() = default;

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

        /**
         * Return norm squared (sometimes its usefull to avoid using sqrt)
         * @return
         */
        double getNorm2() const;
    };

    /**
     * Two Points can be subtracted to get a Vector
     * @param A point
     * @param B point
     * @return vector given by points difference
     */
    Vector operator-(Point A, Point B);

    /** Dump the point representation to stream*/
    std::ostream &operator<<(std::ostream &os, const Point &point);

    /**
     * Number times vector
     * @param k
     * @param a
     * @return scaled vector
     */
    Vector operator*(double k, Vector a);

    /**
     * Vector times number
     * @param a
     * @param k
     * @return
     */
    Vector operator*(Vector a, double k);

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
    Vector operator+(Vector a, Vector b);

    /**
     * Subtract vectors a - b
     * @param a
     * @param b
     * @return
     */
    Vector operator-(Vector a, Vector b);

    /** Dump the vector representation to stream*/
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
        const std::vector<Point *> &getPoints() const;

        /**
         * Construct the element using an id and faces
         *
         * @param id unique identification
         * @param faces
         */
        explicit Element(int id, std::vector<Face *> faces, std::vector<Point *> points);

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

        /** Unique identification */
        int id;
    };

    /**
     * Given an element, calculate and return its centroid
     * @param element
     * @return centroid
     */
    Point getElementCentroid(const Element &element);

    double getElementVolume(const Element &element);
}

#endif //RAYTRACER_GEOMETRY_PRIMITIVES_H
