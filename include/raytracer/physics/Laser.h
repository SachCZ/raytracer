#include <utility>

#ifndef RAYTRACER_LASER_H
#define RAYTRACER_LASER_H

#include <vector>
#include <functional>
#include "Ray.h"
#include "FreeFunctions.h"
#include "Point.h"
#include "Magnitudes.h"

namespace raytracer {
    /**
     * \addtogroup api
     * @{
     */

    /**
         * Find intersections finds all the Intersection of each LaserRay in Laser with Mesh.
         * There are multiple functional parameters to ensure high variability of this method.
         *
         * @note
         * <b>This is the highly modular main method that is the main concern for the user.</b>
         *
         * @tparam DirectionFunction function type
         * @tparam IntersectionFunction function type
         * @tparam StopCondition function type
         * @param mesh to be intersected
         * @param findDirection function that decides new direction every time a Face is encountered.
         * The function must have the following form:
         * @code{.cpp}
            Vector findDirection()(
                const PointOnFace &pointOnFace,
                const Vector &previousDirection,
                const Element &previousElement,
                const Element &nextElement,
                const LaserRay &laserRay
            ) {
                //x, y = ...
                return Vector(x, y);
            }
         * @endcode
         * It is recommended that you copy and paste this to implement findDirection.
         * @param findIntersection function that finds the path of the Ray through given Element and returns
         * PointOnFace where the Ray escapes the Element.
         * The function mush have the following form:
         * @code{.cpp}
            PointOnFace findIntersection(
                const PointOnFace &entryPointOnFace,
                const Vector &entryDirection,
                const Element &element,
                const LaserRay &laserRay
            ) {
                //point, face = ...
                PointOnFace pointOnFace{};
                pointOnFace.point = point;
                pointOnFace.face = face;
                return pointOnFace;
            }
         * @endcode
         * It is recommended that you copy and paste this to implement findIntersection.
         * @param stopCondition function that returns true if the Ray should stop propagation.
         * The function mush have the following form:
         * @code{.cpp}
            bool stopCondition(
                const Element &,
                const LaserRay &laserRay
            ) {
                //shouldStop = ...
                return shouldStop;
            }
         * @endcode
         * It is recommended that you copy and paste this to implement stopCondition.
         */

    using IntersectionSet = std::vector<Intersections>;
    template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
    IntersectionSet generateIntersections(const Mesh &mesh,
                                          const std::vector<HalfLine> &initialDirections,
                                          DirectionFunction findDirection,
                                          IntersectionFunction findIntersection,
                                          StopCondition stopCondition) {
        IntersectionSet result;
        result.reserve(initialDirections.size());

        for (const auto &initialDirection : initialDirections) {
            result.emplace_back(findIntersections(
                    mesh,
                    initialDirection,
                    findDirection,
                    findIntersection,
                    stopCondition
            ));
        }
        return result;
    }

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
    /**
     * @}
     */
}


#endif //RAYTRACER_LASER_H
