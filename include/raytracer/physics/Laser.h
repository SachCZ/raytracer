#include <utility>

#include <utility>

#ifndef RAYTRACER_LASER_H
#define RAYTRACER_LASER_H

#include <vector>
#include <functional>
#include <raytracer/utility/FreeFunctions.h>

#include "Point.h"
#include "LaserRay.h"

namespace raytracer {
    namespace physics {
        class Laser {
            using DirectionFun = std::function<geometry::Vector(geometry::Point)>;
            using EnergyFun = std::function<double(double)>;
        public:
            Laser(Length wavelength, DirectionFun directionFunction, EnergyFun energyFunction) :
                    wavelength(wavelength),
                    directionFunction(std::move(directionFunction)),
                    energyFunction(std::move(energyFunction)) {}

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
