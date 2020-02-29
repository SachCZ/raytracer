#include <utility>

#ifndef RAYTRACER_LASER_H
#define RAYTRACER_LASER_H

#include <vector>
#include <functional>
#include "FreeFunctions.h"
#include "LaserRay.h"
#include "Point.h"

namespace raytracer {
    namespace physics {

        /**
         * Class representing a real physical laser. It is given by its origin, and energy.
         */
        class Laser {
            using DirectionFun = std::function<geometry::Vector(geometry::Point)>;
            using EnergyFun = std::function<double(double)>;
        public:

            /**
             * Construct it using the wavelength in cm, directionFunction, energyFunction and two points in space
             * between which the laser originates.
             *
             * The directionFunction should return the direction of the laser given an arbitrary point in space.
             * The energy function should return the energy of the laser base on the parameter x. X is expected to
             * be meassured from the center of the laser.
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
                    geometry::Point startPoint,
                    geometry::Point endPoint);

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
             * If there are any rays in Laser theit intersections with given mesh will be found.
             * This just calls LaserRay::generateIntersections for each of the rays.
             * @tparam IntersFunc function with signature (Intersection, LaserRay) -> std::unique_ptr<Intersection>
             * @tparam StopCondition function with signature (Intersection, LaserRay) -> bool
             * @param mesh to be intersected
             * @param findInters is propagated to LaserRay::generateIntersections
             * with additional parameter being the laserRay
             * @param stopCondition is propagated to LaserRay::generateIntersections
             * with additional parameter being the laserRay
             */
            template<typename IntersFunc, typename StopCondition>
            void generateIntersections(const geometry::Mesh &mesh,
                                       IntersFunc findInters,
                                       StopCondition stopCondition) {
                if (this->rays.empty()) throw std::logic_error("There are no rays!");

                for (auto &laserRay : this->rays) {
                    auto stopper = [&stopCondition, &laserRay](const geometry::Intersection &intersection) {
                        return stopCondition(intersection, laserRay);
                    };
                    auto finder = [&findInters, &laserRay](const geometry::Intersection& intersection) {
                        return findInters(intersection, laserRay);
                    };
                    laserRay.generateIntersections(mesh, finder, stopper);
                }
            }

            /**
             * Save all the rays to a file in JSON format.
             * There will be one object called rays. It is an array of rays each beeing a sequence of points eg. [0, 1]
             * @param filename name of the json file including extension
             */
            void saveRaysToJson(const std::string &filename);

        private:
            Length wavelength;
            DirectionFun directionFunction;
            EnergyFun energyFunction;
            geometry::Point startPoint;
            geometry::Point endPoint;

            std::vector<LaserRay> rays;
        };
    }
}


#endif //RAYTRACER_LASER_H
