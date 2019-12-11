#include <utility>

#ifndef RAYTRACER_LASER_H
#define RAYTRACER_LASER_H

#include <vector>
#include <functional>
#include "FreeFunctions.h"

#include "Point.h"
#include "Face.h"
#include "LaserRay.h"

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
             */
            Laser(
                    Length wavelength,
                    DirectionFun directionFunction,
                    EnergyFun energyFunction,
                    geometry::Point startPoint,
                    geometry::Point endPoint) :
                    wavelength(wavelength),
                    directionFunction(std::move(directionFunction)),
                    energyFunction(std::move(energyFunction)),
                    startPoint(startPoint),
                    endPoint(endPoint){}

            /** Generate a set of equidistant LaserRays given by the parameters of the whole laser.
             * The rays are generated originating from an edge between a start and end points of the laser.
             * These will be set to the this Laser state.
             *
             *
             * @param count number of rays to be generated
             * @param startPoint from which to start the generation of the rays
             * @param endPoint where to end the generation of the points
             * @return a sequence of rays
             */
            void generateRays(size_t count) {
                this->rays.clear();
                this->rays.reserve(count);
                auto x = utility::linspace(this->startPoint.x, this->endPoint.x, count);
                auto y = utility::linspace(this->startPoint.y, this->endPoint.y, count);

                double sourceWidth = (this->startPoint - this->endPoint).getNorm();
                double parameter = -sourceWidth / 2;
                double deltaParameter = sourceWidth / (count - 1);
                parameter -= deltaParameter / 2; //Integrate with ray in the middle

                for (size_t i = 0; i < count; ++i) {
                    LaserRay laserRay{};
                    laserRay.direction = directionFunction(geometry::Point(x[i], y[i]));
                    laserRay.energy = Energy{utility::integrateTrapz(energyFunction, parameter, deltaParameter)};
                    laserRay.wavelength = this->wavelength;
                    laserRay.startPoint = geometry::Point(x[i], y[i]);

                    this->rays.emplace_back(laserRay);

                    parameter += deltaParameter;
                }
            }

            const std::vector<LaserRay>& getRays(){
                return this->rays;
            }

            template<typename IntersFunc, typename StopCondition>
            void generateIntersections(const geometry::Mesh &mesh,
                                       IntersFunc findInters,
                                       StopCondition stopCondition) {
                if (this->rays.empty()) throw std::logic_error("There are no rays!");

                for (auto &laserRay : this->rays) {
                    laserRay.generateIntersections(mesh, findInters, stopCondition);
                }
            }

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
