#include "absorption.h"

namespace raytracer {
    void EnergyExchangeController::addModel(const EnergyExchangeModel *model) {
        models.emplace_back(model);
    }


    ModelEnergiesSets EnergyExchangeController::genEnergies(
            const IntersectionSet &intersectionSet,
            const Energies &initialEnergies
    ) {
        ModelEnergiesSets result;
        for (const auto &model : this->models) {
            result[model] = EnergiesSet(intersectionSet.size());
            for (size_t setIndex = 0; setIndex < intersectionSet.size(); setIndex++) {
                const auto &intersections = intersectionSet[setIndex];
                result[model][setIndex] = Energies(intersections.size(), Energy{0});
            }
        }
        for (size_t setIndex = 0; setIndex < intersectionSet.size(); setIndex++) {
            const auto &intersections = intersectionSet[setIndex];
            auto currentEnergy = initialEnergies[setIndex].asDouble;
            for (size_t i = 0; i < intersections.size() - 1; i++) {
                const auto &intersection = intersections[i + 1];
                const auto &prevIntersection = intersections[i];

                for (const auto &model : this->models) {
                    auto absorbed = model->getEnergyChange(
                            prevIntersection,
                            intersection,
                            Energy{currentEnergy});
                    currentEnergy -= absorbed.asDouble;
                    result[model][setIndex][i + 1] = absorbed;
                }
            }
        }
        return result;
    }

    Energy Resonance::getEnergyChange(const Intersection &previousIntersection, const Intersection &currentIntersection,
                                      const Energy &currentEnergy) const {
        if (!Resonance::isResonating(*currentIntersection.previousElement, currentIntersection.pointOnFace))
            return Energy{0};

        auto grad = gradientCalculator(
                currentIntersection.pointOnFace,
                *currentIntersection.previousElement,
                *currentIntersection.nextElement
        );
        auto dir = (currentIntersection.pointOnFace.point - previousIntersection.pointOnFace.point);
        auto q = Resonance::getQ(dir, grad);
        auto term = q * std::exp(-4.0 / 3.0 * std::pow(q, 3.0 / 2.0)) / (q + 0.48) * M_PI / 2.0;
        return Energy{currentEnergy.asDouble * term};
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
            const Gradient &gradientCalculator,
            const Length &wavelength,
            const Marker &reflectedMarker) :
            gradientCalculator(gradientCalculator),
            wavelength(wavelength),
            reflectedMarker(reflectedMarker) {}

    std::string Resonance::getName() const {
        return "Resonance";
    }

    Bremsstrahlung::Bremsstrahlung(const MeshFunc &bremssCoeff) : bremssCoeff(bremssCoeff) {}

    Energy Bremsstrahlung::getEnergyChange(
            const Intersection &previousIntersection,
            const Intersection &currentIntersection,
            const Energy &currentEnergy
    ) const {
        const auto &element = currentIntersection.previousElement;
        if (!element) return Energy{0};
        const auto &previousPoint = previousIntersection.pointOnFace.point;
        const auto &point = currentIntersection.pointOnFace.point;

        const auto distance = (point - previousPoint).getNorm();
        auto coeff = bremssCoeff.getValue(*element);
        const auto exponent = -coeff * distance;

        auto newEnergy = currentEnergy.asDouble * std::exp(exponent);
        return Energy{currentEnergy.asDouble - newEnergy};
    }

    std::string Bremsstrahlung::getName() const {
        return "Bremsstrahlung";
    }

    std::string stringifyAbsorptionSummary(const AbsorptionSummary &summary) {
        std::stringstream stream;
        double total = 0;
        const auto &modelsEnergies = summary.modelEnergies;
        const auto initialEnergy = summary.initialEnergy;
        for (auto const &modelEnergy : modelsEnergies) {
            const auto &model = modelEnergy.first;
            const auto &energy = modelEnergy.second;

            total += energy.asDouble;
            stream << model->getName() << ": " << energy.asDouble << " ... "
                   << energy.asDouble / initialEnergy.asDouble * 100 << "%" << std::endl;
        }
        stream << "Total: " << total << " ... "
               << total / initialEnergy.asDouble * 100 << "%" << std::endl;
        return stream.str();
    }

    void absorbRayEnergies(MeshFunc &absorbedEnergy, const EnergiesSet &energiesSets,
                           const IntersectionSet &intersectionSet) {
        for (size_t setIndex = 0; setIndex < intersectionSet.size(); setIndex++) {
            const auto &energies = energiesSets[setIndex];
            const auto &intersections = intersectionSet[setIndex];
            for (size_t i = 1; i < intersections.size(); i++) {
                auto element = intersections[i].previousElement;
                if (!element) continue;
                const auto &absorbed = -(energies[i].asDouble - energies[i - 1].asDouble);
                absorbedEnergy.addValue(*element, absorbed);
            }
        }
    }

    EnergiesSet modelEnergiesToRayEnergies(const ModelEnergiesSets &modelEnergiesSets, const Energies &initialEnergies) {
        auto setsCount = modelEnergiesSets.begin()->second.size();
        EnergiesSet result;
        result.reserve(setsCount);

        const auto& firstEnergiesSets = modelEnergiesSets.begin()->second;
        for (const auto& energies : firstEnergiesSets) {
            result.emplace_back(Energies(energies.size(), Energy{0}));
        }
        for (const auto& oneModelEnergiesSets : modelEnergiesSets) {
            const auto& energiesSets = oneModelEnergiesSets.second;
            for (size_t setIndex = 0; setIndex < setsCount; setIndex++){
                const auto& energies = energiesSets[setIndex];
                for (size_t i = 0; i < energies.size(); i++){
                    result[setIndex][i].asDouble += energies[i].asDouble;
                }
            }
        }

        for (size_t setIndex = 0; setIndex < setsCount; setIndex++){
            auto& energies = result[setIndex];
            double currentEnergy = initialEnergies[setIndex].asDouble;
            for (auto & energy : energies ){
                currentEnergy -= energy.asDouble;
                energy = Energy{currentEnergy};
            }
        }

        return result;
    }

    std::vector<std::vector<double>> genSetSerialization(const EnergiesSet &energiesSet) {
        std::vector<std::vector<double>> result;
        result.reserve(energiesSet.size());
        for (const auto& energies : energiesSet){
            std::vector<double> rayEnergySerialization;
            rayEnergySerialization.reserve(energies.size());
            for (auto energy : energies) {
                rayEnergySerialization.emplace_back(energy.asDouble);
            }
            result.emplace_back(rayEnergySerialization);
        }
        return result;
    }

    std::ostream &modelEnergiesToMsgpack(const ModelEnergiesSets &modelEnergiesSets, std::ostream &os) {
        std::map<std::string, std::vector<std::vector<double>>> energiesSerialization;
        for (const auto &oneModelEnergiesSet : modelEnergiesSets) {
            auto modelName = oneModelEnergiesSet.first->getName();
            auto energiesSet = oneModelEnergiesSet.second;

            energiesSerialization[modelName] = genSetSerialization(energiesSet);
        }
        msgpack::pack(os, energiesSerialization);
        return os;
    }

    std::ostream &rayEnergiesToMsgpack(const EnergiesSet &energiesSet, std::ostream &os) {
        msgpack::pack(os, genSetSerialization(energiesSet));
        return os;
    }

    XRayGain::XRayGain(const MeshFunc &gain) : gain(gain) {}

    raytracer::Energy
    XRayGain::getEnergyChange(const Intersection &previousIntersection, const Intersection &currentIntersection,
                              const Energy &currentEnergy) const {
        auto distance = (currentIntersection.pointOnFace.point - previousIntersection.pointOnFace.point).getNorm();
        auto element = currentIntersection.previousElement;
        auto gainCoeff = gain.getValue(*element);
        return raytracer::Energy{currentEnergy.asDouble * (1 - std::exp(gainCoeff * distance))};
    }

    std::string XRayGain::getName() const {
        return "X-ray gain";
    }
}

