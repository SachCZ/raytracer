#ifndef RAYTRACER_LASER_H
#define RAYTRACER_LASER_H

#include <vector>
#include <functional>
#include "magnitudes.h"
#include <utility>
#include <geometry.h>

namespace raytracer {

    /**
     * Class representing a real physical laser. It is given by its energy and origin (it is a line segment)).
     */
    struct Laser {
        /** Function specifing the direction of the laser at origin points */
        using DirectionFun = std::function<Vector(Point)>;
        /** Function specifing the energy of the laser at origin points */
        using EnergyFun = std::function<double(double)>;

        /** Wavelenght in cm */
        Length wavelength;
        /** Function of type Laser::DirectionFun */
        DirectionFun directionFunction;
        /** Function of type Laser::EnergyFun */
        EnergyFun energyFunction;
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

    /** Sequence of energies */
    using Energies = std::vector<Energy>;

    /** std::vector of of Energies instances */
    using EnergiesSet = std::vector<Energies>;

    /**
     * Take a laser and generate initial energies corresponding to initial rays
     * @param laser
     * @return
     */
    Energies generateInitialEnergies(const Laser &laser);

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
