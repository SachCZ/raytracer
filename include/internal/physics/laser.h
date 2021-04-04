#ifndef RAYTRACER_LASER_H
#define RAYTRACER_LASER_H

#include <vector>
#include <functional>
#include "magnitudes.h"
#include <utility>
#include <geometry.h>

namespace raytracer {

    /**
     * Class representing a real physical laser. It is given by its power and origin (it is a line segment)).
     */
    struct Laser {
        /** Function specifing the direction of the laser at origin points */
        using DirectionFun = std::function<Vector(Point)>;
        /** Function specifing the power of the laser at origin points */
        using PowerFun = std::function<double(double)>;

        /** Wavelenght in cm */
        Length wavelength;
        /** Function of type Laser::DirectionFun */
        DirectionFun directionFunction;
        /** Function of type Laser::PowerFun */
        PowerFun powerFunction;
        /** Start point of origin line segment */
        Point startPoint;
        /** End point of origin line segment */
        Point endPoint;
        /** The number of individual rays the laser consists of in the geometric optics approximation */
        int raysCount;
    };

    /**
     * Take a laser and generate initial rays of the laser
     * @param laser
     * @return rays
     */
    std::vector<Ray> generateInitialDirections(const Laser &laser);

    /** Sequence of powers */
    using Powers = std::vector<Power>;

    /** std::vector of of Powers instances */
    using PowersSet = std::vector<Powers>;

    /**
     * Take a laser and generate initial powers corresponding to initial rays
     * @param laser
     * @return
     */
    Powers generateInitialPowers(const Laser &laser);

    /**
     * Take intersections and dump them to JSON string
     * @param intersectionSet
     * @return
     */
    std::string stringifyRaysToJson(const IntersectionSet& intersectionSet);

    /**
     * Take intersections and dump them to msgpack binary format string
     * @param intersectionSet
     * @return
     */
    std::string stringifyRaysToMsgpack(const IntersectionSet& intersectionSet);
}


#endif //RAYTRACER_LASER_H
