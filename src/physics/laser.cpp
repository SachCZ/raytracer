#include "laser.h"
#include <fstream>
#include <json/json.h>
#include <msgpack.hpp>
#include <utility.h>


namespace raytracer {
    Laser::Laser(Length wavelength,
                 Laser::DirectionFun directionFunction,
                 Laser::EnergyFun energyFunction,
                 Point startPoint, Point endPoint, int raysCount) :
            wavelength(wavelength),
            directionFunction(std::move(directionFunction)),
            energyFunction(std::move(energyFunction)),
            startPoint(startPoint),
            endPoint(endPoint),
            raysCount(raysCount) {}

    using Directions = std::vector<Ray>;
    Directions generateInitialDirections(const Laser &laser) {
        Directions result;
        result.reserve(laser.raysCount);
        auto x = linspace(laser.startPoint.x, laser.endPoint.x, laser.raysCount);
        auto y = linspace(laser.startPoint.y, laser.endPoint.y, laser.raysCount);

        for (int i = 0; i < laser.raysCount; ++i) {
            Point point(x[i], y[i]);
            result.emplace_back(Ray{point, laser.directionFunction(point)});
        }
        return result;
    }

    std::string stringifyRaysToJson(const IntersectionSet& intersectionSet) {
        Json::Value root;

        root["rays"] = Json::Value(Json::arrayValue);
        for (const auto &intersections : intersectionSet) {
            Json::Value rayJson = Json::Value(Json::arrayValue);

            for (const auto &intersection : intersections) {
                Json::Value pointJson;
                pointJson[0] = intersection.pointOnFace.point.x;
                pointJson[1] = intersection.pointOnFace.point.y;

                rayJson.append(pointJson);
            }

            root["rays"].append(rayJson);
        }
        std::stringstream result;
        result << root;
        return result.str();
    }

    std::string stringifyRaysToMsgpack(const IntersectionSet& intersectionSet) {
        std::vector<std::map<std::string, std::vector<double>>> raysSerialization;
        for (const auto &intersections : intersectionSet) {
            std::vector<double> x;
            std::vector<double> y;
            for (const auto &intersection : intersections) {
                x.emplace_back(intersection.pointOnFace.point.x);
                y.emplace_back(intersection.pointOnFace.point.y);
            }
            std::map<std::string, std::vector<double>> raySerialization;
            raySerialization["x"] = x;
            raySerialization["y"] = y;
            raysSerialization.emplace_back(raySerialization);
        }
        std::stringstream result;
        msgpack::pack(result, raysSerialization);
        return result.str();
    }

    EnergiesSet generateInitialEnergies(const Laser &laser) {
        EnergiesSet result;
        double sourceWidth = (laser.startPoint - laser.endPoint).getNorm();
        double parameter = -sourceWidth / 2;
        double deltaParameter = sourceWidth / laser.raysCount;
        parameter -= deltaParameter / 2; //Integrate with ray in the middle

        for (int i = 0; i < laser.raysCount; ++i) {
            result.emplace_back(Energy{integrateTrapz(laser.energyFunction, parameter, deltaParameter)});
            parameter += deltaParameter;
        }
        return result;
    }
}

