#ifndef RAYTRACER_LASER_H
#define RAYTRACER_LASER_H

#include <vector>
#include <functional>
#include "magnitudes.h"
#include <utility>
#include <geometry.h>

namespace raytracer {

    using DirectionFun = std::function<Vector(Point)>;
    using EnergyFun = std::function<double(double)>;
    /**
     * Class representing a real physical laser. It is given by its origin, and energy.
     */
    struct Laser {
        Length wavelength;
        DirectionFun directionFunction;
        EnergyFun energyFunction;
        Point startPoint;
        Point endPoint;
        int raysCount;
    };

    /**
     * Take a laser and generate initial rays of the laser
     * @param laser
     * @return rays
     */
    std::vector<Ray> generateInitialDirections(const Laser &laser);

    using EnergiesSet = std::vector<Energy>;

    /**
     * Take a laser and generate initial energies corresponding to initial rays
     * @param laser
     * @return
     */
    EnergiesSet generateInitialEnergies(const Laser &laser);

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
