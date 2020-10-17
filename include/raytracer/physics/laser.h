#ifndef RAYTRACER_LASER_H
#define RAYTRACER_LASER_H

#include <vector>
#include <functional>
#include "magnitudes.h"
#include <utility>
#include <geometry.h>

namespace raytracer {

    /**
     * Class representing a real physical laser. It is given by its origin, and energy.
     */
    class Laser {
        using DirectionFun = std::function<Vector(Point)>;
        using EnergyFun = std::function<double(double)>;

    public:
        Laser(
                Length wavelength,
                DirectionFun directionFunction,
                EnergyFun energyFunction,
                Point startPoint,
                Point endPoint,
                int raysCount
        );

        Length wavelength;
        DirectionFun directionFunction;
        EnergyFun energyFunction;
        Point startPoint;
        Point endPoint;
        int raysCount;
    };

    /** Generate a set of equidistant LaserRays given by the parameters of the whole laser.
         * The rays are generated originating from an edge between a start and end points of the laser.
         * These will be set to the this Laser state.
         *
         * @param count number of rays to be generated
         * @return a sequence of rays
         */

    std::vector<HalfLine> generateInitialDirections(const Laser &laser);

    using EnergiesSet = std::vector<Energy>;
    EnergiesSet generateInitialEnergies(const Laser &laser);

    std::string stringifyRaysToJson(const IntersectionSet& intersectionSet);

    std::string stringifyRaysToMsgpack(const IntersectionSet& intersectionSet);
}


#endif //RAYTRACER_LASER_H
