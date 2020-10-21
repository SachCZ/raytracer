#include "absorption.h"

namespace raytracer {
    void EnergyExchangeController::addModel(const EnergyExchangeModel *model) {
        models.emplace_back(model);
    }


    AbsorptionSummary EnergyExchangeController::absorb(
            const IntersectionSet &intersectionSet,
            const Energies &initialEnergies,
            MeshFunc &absorbedEnergy
    ) {
        ModelEnergies modelEnergies;
        Energy initialEnergy{0};
        for (size_t i = 0; i < intersectionSet.size(); i++) {
            initialEnergy.asDouble += initialEnergies[i].asDouble;
            auto modelsEnergies = this->absorbLaserRay(intersectionSet[i], initialEnergies[i], absorbedEnergy);
            for (auto const &modelEnergy : modelsEnergies) {
                const auto &model = modelEnergy.first;
                const auto &energy = modelEnergy.second;

                if (modelEnergies.find(model) != modelEnergies.end()) {
                    modelEnergies[model] = Energy{energy.asDouble + modelEnergies[model].asDouble};
                } else {
                    modelEnergies[model] = energy;
                }
            }

        }
        return {modelEnergies, initialEnergy};
    }

    ModelEnergies EnergyExchangeController::absorbLaserRay(
            const Intersections &intersections,
            const Energy &initialEnergy,
            MeshFunc &absorbedEnergy
    ) {
        auto intersectionIt = std::next(std::begin(intersections));
        auto previousIntersectionIt = std::begin(intersections);

        auto currentEnergy = initialEnergy.asDouble;

        std::map<const EnergyExchangeModel *, Energy> result;

        for (; intersectionIt != std::end(intersections); ++intersectionIt, ++previousIntersectionIt) {
            for (const auto &model : this->models) {
                auto absorbed = model->getEnergyChange(
                        *previousIntersectionIt,
                        *intersectionIt,
                        Energy{currentEnergy}).asDouble;
                currentEnergy -= absorbed;
                if (result.find(model) != result.end()) {
                    result[model] = Energy{absorbed + result[model].asDouble};
                } else {
                    result[model] = Energy{absorbed};
                }
                absorbedEnergy.addValue(*(intersectionIt->previousElement), absorbed);
            }
        }
        return result;
    }

    Energy Resonance::getEnergyChange(const Intersection &previousIntersection, const Intersection &currentIntersection,
                                      const Energy &currentEnergy) const {
        if (!Resonance::isResonating(*currentIntersection.previousElement, currentIntersection.pointOnFace))
            return Energy{0};

        auto grad = gradientCalculator.get(
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

