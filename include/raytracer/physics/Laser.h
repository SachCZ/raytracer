#ifndef RAYTRACER_LASER_H
#define RAYTRACER_LASER_H

#include <vector>
#include <functional>
#include "FreeFunctions.h"

#include "Point.h"
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
             * Construct it using the wavelength in cm, directionFunction and energyFunction.
             *
             * The directionFunction should return the direction of the laser given an arbitrary point in space.
             * The energy function should return the energy of the laser base on the parameter x. X is expected to
             * be meassured from the center of the laser.
             *
             * @param wavelength of the laser in cm
             * @param directionFunction function with signature (const Point& point) -> Vector
             * @param energyFunction function with signature (double x) -> double.
             */
            Laser(Length wavelength, DirectionFun directionFunction, EnergyFun energyFunction) :
                    wavelength(wavelength),
                    directionFunction(std::move(directionFunction)),
                    energyFunction(std::move(energyFunction)) {}

            /** Generate a set of equidistant LaserRays given by the parameters of the whole laser.
             * The rays are generated originating from an edge between a start and end points.
             *
             * @param count number of rays to be generated
             * @param startPoint from which to start the generation of the rays
             * @param endPoint where to end the generation of the points
             * @return a sequence of rays
             */
            std::vector<LaserRay> generateRays(size_t count, geometry::Point startPoint, geometry::Point endPoint) {
                std::vector<LaserRay> result;
                result.reserve(count);
                auto x = utility::linspace(startPoint.x, endPoint.x, count);
                auto y = utility::linspace(startPoint.y, endPoint.y, count);

                double sourceWidth = (startPoint - endPoint).getNorm();
                double parameter = -sourceWidth / 2;
                double deltaParameter = sourceWidth / (count - 1);
                parameter -= deltaParameter / 2; //Integrate with ray in the middle

                for (size_t i = 0; i < count; ++i) {
                    LaserRay laserRay{};
                    laserRay.direction = directionFunction(geometry::Point(x[i], y[i]));
                    laserRay.energy = Energy{utility::integrateTrapz(energyFunction, parameter, deltaParameter)};
                    laserRay.wavelength = this->wavelength;
                    laserRay.startPoint = geometry::Point(x[i], y[i]);

                    result.emplace_back(laserRay);

                    parameter += deltaParameter;
                }
                return result;
            }

        private:
            Length wavelength;
            DirectionFun directionFunction;
            EnergyFun energyFunction;
        };
    }
}


#endif //RAYTRACER_LASER_H
