#include "absorption.h"

#include <utility>

namespace raytracer {
    void PowerExchangeController::addModel(const PowerExchangeModel *model) {
        models.emplace_back(model);
    }


    ModelPowersSets PowerExchangeController::genPowers(
            const IntersectionSet &intersectionSet,
            const Powers &initialPowers
    ) const {
        ModelPowersSets result;
        for (const auto &model : this->models) {
            result[model] = PowersSet(intersectionSet.size());
            for (size_t setIndex = 0; setIndex < intersectionSet.size(); setIndex++) {
                const auto &intersections = intersectionSet[setIndex];
                result[model][setIndex] = Powers(intersections.size(), Power{0});
            }
        }
        for (size_t setIndex = 0; setIndex < intersectionSet.size(); setIndex++) {
            const auto &intersections = intersectionSet[setIndex];
            auto currentPower = initialPowers[setIndex].asDouble;
            for (size_t i = 0; i < intersections.size() - 1; i++) {
                const auto &intersection = intersections[i + 1];
                const auto &prevIntersection = intersections[i];

                for (const auto &model : this->models) {
                    auto absorbed = model->getPowerChange(
                            prevIntersection,
                            intersection,
                            Power{currentPower});
                    currentPower -= absorbed.asDouble;
                    result[model][setIndex][i + 1] = absorbed;
                }
            }
        }
        return result;
    }

    size_t PowerExchangeController::getModelsCount() const {
        return this->models.size();
    }

    Power Resonance::getPowerChange(const Intersection &previousIntersection, const Intersection &currentIntersection,
                                      const Power &currentPower) const {
        if (!Resonance::isResonating(*currentIntersection.previousElement, currentIntersection.pointOnFace))
            return Power{0};
        Vector grad{};
        try {
            grad = gradientCalculator(
                    currentIntersection.pointOnFace,
                    *currentIntersection.previousElement,
                    *currentIntersection.nextElement
            );
        } catch (const std::logic_error& err) {
            //std::cout << "No grad found" << std::endl;
            return Power{0};
        }

        auto dir = (currentIntersection.pointOnFace.point - previousIntersection.pointOnFace.point);
        auto q = Resonance::getQ(dir, grad);
        auto term = q * std::exp(-4.0 / 3.0 * std::pow(q, 3.0 / 2.0)) / (q + 0.48) * M_PI / 2.0;
        return Power{currentPower.asDouble * term};
    }

    bool Resonance::isResonating(const Element &element, const PointOnFace &pointOnFace) const {
        return reflectedMarker.isMarked(element, pointOnFace);
    }

    double Resonance::getQ(Vector dir, Vector grad) const {
        auto dir_norm = dir.getNorm();
        if (dir_norm == 0) return 0;
        auto grad_norm = grad.getNorm();
        if (grad_norm == 0) return 0;
        auto lamb = wavelength.asDouble;
        auto ne_crit = calcCritDens(wavelength).asDouble;
        auto sin2phi = 1 - std::pow(grad * dir / grad_norm / dir_norm, 2);
        if (sin2phi <= 0) return 0;
        return std::pow(2 * M_PI / lamb * ne_crit / grad_norm, 2.0 / 3.0) * sin2phi;
    }

    Resonance::Resonance(
            Gradient gradientCalculator,
            const Length &wavelength,
            const Marker &reflectedMarker) :
            gradientCalculator(std::move(gradientCalculator)),
            wavelength(wavelength),
            reflectedMarker(reflectedMarker) {}

    std::string Resonance::getName() const {
        return "Resonance";
    }

    Bremsstrahlung::Bremsstrahlung(const MeshFunc &bremssCoeff) : bremssCoeff(bremssCoeff) {}

    Power Bremsstrahlung::getPowerChange(
            const Intersection &previousIntersection,
            const Intersection &currentIntersection,
            const Power &currentPower
    ) const {
        const auto &element = currentIntersection.previousElement;
        if (!element) return Power{0};
        const auto &previousPoint = previousIntersection.pointOnFace.point;
        const auto &point = currentIntersection.pointOnFace.point;

        const auto distance = (point - previousPoint).getNorm();
        auto coeff = bremssCoeff.getValue(*element);
        const auto exponent = -coeff * distance;

        auto newPower = currentPower.asDouble * std::exp(exponent);
        return Power{currentPower.asDouble - newPower};
    }

    std::string Bremsstrahlung::getName() const {
        return "Bremsstrahlung";
    }

    std::string stringifyAbsorptionSummary(const AbsorptionSummary &summary) {
        std::stringstream stream;
        double total = 0;
        const auto &modelsPowers = summary.modelPowers;
        const auto initialPower = summary.initialPower;
        for (auto const &modelPower : modelsPowers) {
            const auto &model = modelPower.first;
            const auto &power = modelPower.second;

            total += power.asDouble;
            stream << model->getName() << ": " << power.asDouble << " ... "
                   << power.asDouble / initialPower.asDouble * 100 << "%" << std::endl;
        }
        stream << "Total: " << total << " ... "
               << total / initialPower.asDouble * 100 << "%" << std::endl;
        return stream.str();
    }

    void absorbRayPowers(MeshFunc &absorbedPower, const PowersSet &powersSets,
                           const IntersectionSet &intersectionSet) {
        for (size_t setIndex = 0; setIndex < intersectionSet.size(); setIndex++) {
            const auto &powers = powersSets[setIndex];
            const auto &intersections = intersectionSet[setIndex];
            for (size_t i = 1; i < intersections.size(); i++) {
                auto element = intersections[i].previousElement;
                if (!element) continue;
                const auto &absorbed = -(powers[i].asDouble - powers[i - 1].asDouble);
                absorbedPower.addValue(*element, absorbed);
            }
        }
    }

    PowersSet modelPowersToRayPowers(const ModelPowersSets &modelPowersSets, const Powers &initialPowers) {
        auto setsCount = modelPowersSets.begin()->second.size();
        PowersSet result;
        result.reserve(setsCount);

        const auto& firstPowersSets = modelPowersSets.begin()->second;
        for (const auto& powers : firstPowersSets) {
            result.emplace_back(Powers(powers.size(), Power{0}));
        }
        for (const auto& oneModelPowersSets : modelPowersSets) {
            const auto& powersSets = oneModelPowersSets.second;
            for (size_t setIndex = 0; setIndex < setsCount; setIndex++){
                const auto& powers = powersSets[setIndex];
                for (size_t i = 0; i < powers.size(); i++){
                    result[setIndex][i].asDouble += powers[i].asDouble;
                }
            }
        }

        for (size_t setIndex = 0; setIndex < setsCount; setIndex++){
            auto& powers = result[setIndex];
            double currentPower = initialPowers[setIndex].asDouble;
            for (auto & power : powers ){
                currentPower -= power.asDouble;
                power = Power{currentPower};
            }
        }

        return result;
    }

    std::vector<std::vector<double>> genSetSerialization(const PowersSet &powersSet) {
        std::vector<std::vector<double>> result;
        result.reserve(powersSet.size());
        for (const auto& powers : powersSet){
            std::vector<double> rayPowerSerialization;
            rayPowerSerialization.reserve(powers.size());
            for (auto power : powers) {
                rayPowerSerialization.emplace_back(power.asDouble);
            }
            result.emplace_back(rayPowerSerialization);
        }
        return result;
    }

    std::ostream &modelPowersToMsgpack(const ModelPowersSets &modelPowersSets, std::ostream &os) {
        std::map<std::string, std::vector<std::vector<double>>> powersSerialization;
        for (const auto &oneModelPowersSet : modelPowersSets) {
            auto modelName = oneModelPowersSet.first->getName();
            auto powersSet = oneModelPowersSet.second;

            powersSerialization[modelName] = genSetSerialization(powersSet);
        }
        msgpack::pack(os, powersSerialization);
        return os;
    }

    std::ostream &rayPowersToMsgpack(const PowersSet &powersSet, std::ostream &os) {
        msgpack::pack(os, genSetSerialization(powersSet));
        return os;
    }

    XRayGain::XRayGain(const MeshFunc &gain) : gain(gain) {}

    raytracer::Power
    XRayGain::getPowerChange(const Intersection &previousIntersection, const Intersection &currentIntersection,
                              const Power &currentPower) const {
        auto distance = (currentIntersection.pointOnFace.point - previousIntersection.pointOnFace.point).getNorm();
        auto element = currentIntersection.previousElement;
        auto gainCoeff = gain.getValue(*element);
        return raytracer::Power{currentPower.asDouble * (1 - std::exp(gainCoeff * distance))};
    }

    std::string XRayGain::getName() const {
        return "X-ray gain";
    }
}

