#include "Laser.h"
#include <fstream>
#include <json/json.h>

namespace raytracer {
    namespace physics {
        Laser::Laser(Length wavelength,
                     Laser::DirectionFun directionFunction,
                     Laser::EnergyFun energyFunction,
                     geometry::Point startPoint, geometry::Point endPoint) :
                wavelength(wavelength),
                directionFunction(std::move(directionFunction)),
                energyFunction(std::move(energyFunction)),
                startPoint(startPoint),
                endPoint(endPoint) {}

        void Laser::generateRays(size_t count) {
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

        const std::vector<LaserRay> &Laser::getRays() const {
            return this->rays;
        }

        void Laser::saveRaysToJson(const std::string &filename) {
            Json::Value root;

            root["rays"] = Json::Value(Json::arrayValue);
            for (const auto &ray : this->getRays()) {
                Json::Value rayJson = Json::Value(Json::arrayValue);

                for (const auto &intersection : ray.intersections) {
                    Json::Value pointJson;
                    pointJson[0] = intersection.pointOnFace.point.x;
                    pointJson[1] = intersection.pointOnFace.point.y;

                    rayJson.append(pointJson);
                }

                root["rays"].append(rayJson);
            }
            std::ofstream file(filename);
            file << root;
        }
    }
}

