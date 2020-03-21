#include <utility>

#ifndef RAYTRACER_LASER_H
#define RAYTRACER_LASER_H

#include <vector>
#include <functional>
#include "FreeFunctions.h"
#include "LaserRay.h"
#include "Point.h"

namespace raytracer {
    /**
     * \addtogroup api
     * @{
     */

    /**
     * Class representing a real physical laser. It is given by its origin, and energy.
     */
    class Laser {
        using DirectionFun = std::function<Vector(Point)>;
        using EnergyFun = std::function<double(double)>;
    public:

        /**
         * Construct it using the wavelength in cm, directionFunction, energyFunction and two points in space
         * between which the laser originates.
         *
         * The directionFunction should return the direction of the laser given an arbitrary point in space.
         * The energy function should return the energy of the laser base on the parameter x. X is expected to
         * be measured from the center of the laser.
         *
         * @param wavelength of the laser in cm
         * @param directionFunction function with signature (const Point& point) -> Vector
         * @param energyFunction function with signature (double x) -> double.
         * @param startPoint from which to start the generation of the rays
         * @param endPoint where to end the generation of the points
         */
        Laser(
                Length wavelength,
                DirectionFun directionFunction,
                EnergyFun energyFunction,
                Point startPoint,
                Point endPoint
                );

        /** Generate a set of equidistant LaserRays given by the parameters of the whole laser.
         * The rays are generated originating from an edge between a start and end points of the laser.
         * These will be set to the this Laser state.
         *
         * @param count number of rays to be generated
         * @return a sequence of rays
         */
        void generateRays(size_t count);

        /**
         * Get all the rays in that are in the current state of Laser
         * @return all rays
         */
        const std::vector<LaserRay> &getRays() const;

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
        template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
        void generateIntersections(const Mesh &mesh,
                                   DirectionFunction findDirection,
                                   IntersectionFunction findIntersection,
                                   StopCondition stopCondition) {
            if (this->rays.empty()) throw std::logic_error("There are no rays!");

            for (auto &laserRay : this->rays) {
                auto stopper = [&stopCondition, &laserRay](const Element &nextElement) {
                    return stopCondition(nextElement, laserRay);
                };
                auto finder = [&findIntersection, &laserRay](
                        const PointOnFace &pointOnFace,
                        const Vector &direction,
                        const Element &nextElement
                ) {
                    return findIntersection(pointOnFace, direction, nextElement, laserRay);
                };
                auto directioner = [&findDirection, &laserRay](
                        const PointOnFace &pointOnFace,
                        const Vector &previousDirection,
                        const Element &previousElement,
                        const Element &nextElement
                ) {
                    return findDirection(pointOnFace, previousDirection, previousElement, nextElement, laserRay);
                };
                laserRay.generateIntersections(mesh, directioner, finder, stopper);
            }
        }

        /**
         * Save all the rays to a file in JSON format.
         * There will be one object called rays. It is an array of rays each being a sequence of points eg. (0, 1)
         * @param filename name of the json file including extension
         */
        void saveRaysToJson(const std::string &filename);

    private:
        Length wavelength;
        DirectionFun directionFunction;
        EnergyFun energyFunction;
        Point startPoint;
        Point endPoint;

        std::vector<LaserRay> rays;
    };

    /**
     * @}
     */
}


#endif //RAYTRACER_LASER_H
