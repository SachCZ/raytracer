#include "Absorption.h"

namespace raytracer {
    void AbsorptionController::addModel(const AbsorptionModel *model) {
        models.emplace_back(model);
    }

    std::map<const AbsorptionModel *, Energy> AbsorptionController::absorb(
            const Laser &laser,
            MeshFunction &absorbedEnergy
    ) {
        std::map<const AbsorptionModel *, Energy> result;
        for (const auto &laserRay : laser.getRays()) {
            auto modelsEnergies = this->absorbLaserRay(laserRay, absorbedEnergy);
            for (auto const &modelEnergy : modelsEnergies) {
                const auto &model = modelEnergy.first;
                const auto &energy = modelEnergy.second;

                if (result.find(model) != result.end()) {
                    result[model] = Energy{energy.asDouble + result[model].asDouble};
                } else {
                    result[model] = energy;
                }
            }

        }
        return result;
    }

    std::map<const AbsorptionModel *, Energy> AbsorptionController::absorbLaserRay(const LaserRay &laserRay,
                                                                                   MeshFunction &absorbedEnergy) {
        const auto &intersections = laserRay.intersections;
        auto intersectionIt = std::next(std::begin(intersections));
        auto previousIntersectionIt = std::begin(intersections);

        auto currentEnergy = laserRay.energy.asDouble;

        std::map<const AbsorptionModel *, Energy> result;

        for (; intersectionIt != std::end(intersections); ++intersectionIt, ++previousIntersectionIt) {
            for (const auto &model : this->models) {
                auto absorbed = model->getEnergyChange(
                        *previousIntersectionIt,
                        *intersectionIt,
                        Energy{currentEnergy},
                        laserRay
                ).asDouble;
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

    Energy Resonance::getEnergyChange(
            const Intersection &previousIntersection,
            const Intersection &currentIntersection,
            const Energy &currentEnergy,
            const LaserRay &laserRay
    ) const {
        if (!Resonance::isResonating(*currentIntersection.previousElement, laserRay)) return Energy{0};

        auto grad = gradientCalculator.get(
                currentIntersection.pointOnFace,
                *currentIntersection.previousElement,
                *currentIntersection.nextElement
        );
        auto dir = (currentIntersection.pointOnFace.point - previousIntersection.pointOnFace.point);
        auto q = Resonance::getQ(laserRay, dir, grad);
        auto term = q * std::exp(-4.0 / 3.0 * std::pow(q, 3.0 / 2.0)) / (q + 0.48) * M_PI / 2.0;
        return Energy{currentEnergy.asDouble * term};
    }

    bool Resonance::isResonating(const Element &element, const LaserRay &laserRay) const {
        return reflectedMarker.isMarked(element, laserRay);
    }

    double Resonance::getQ(const LaserRay &laserRay, Vector dir, Vector grad) {
        auto dir_norm = dir.getNorm();
        if (dir_norm == 0) return 0;
        auto grad_norm = grad.getNorm();
        if (grad_norm == 0) return 0;
        auto lamb = laserRay.wavelength.asDouble;
        auto ne_crit = laserRay.getCriticalDensity().asDouble;
        auto sin2phi = 1 - std::pow(grad * dir / grad_norm / dir_norm, 2);
        if (sin2phi <= 0) return 0;
        return std::pow(2 * M_PI / lamb * ne_crit / grad_norm, 2.0 / 3.0) * sin2phi;
    }

    Resonance::Resonance(const Gradient &gradientCalculator, const Marker &reflectedMarker) :
            gradientCalculator(gradientCalculator), reflectedMarker(reflectedMarker) {}

    Bremsstrahlung::Bremsstrahlung(const MeshFunction &density, const MeshFunction &temperature,
                                   const MeshFunction &ionization, const CollisionalFrequency &collisionalFrequency) :
            _density(density),
            _temperature(temperature),
            _ionization(ionization),
            collisionalFrequency(collisionalFrequency) {}

    Energy Bremsstrahlung::getEnergyChange(
            const Intersection &previousIntersection,
            const Intersection &currentIntersection,
            const Energy &currentEnergy,
            const LaserRay &laserRay
    ) const {
        const auto &element = currentIntersection.previousElement;
        if (!element) return Energy{0};
        const auto &previousPoint = previousIntersection.pointOnFace.point;
        const auto &point = currentIntersection.pointOnFace.point;

        const auto distance = (point - previousPoint).getNorm();
        const auto density = Density{this->_density.getValue(*element)};
        const auto temperature = Temperature{this->_temperature.getValue(*element)};
        const auto ionization = this->_ionization.getValue(*element);

        auto frequency = collisionalFrequency.get(density, temperature, laserRay.wavelength, ionization);
        auto coeff = laserRay.getInverseBremsstrahlungCoeff(density, frequency);
        const auto exponent = -coeff * distance;

        auto newEnergy = currentEnergy.asDouble * std::exp(exponent);
        return Energy{currentEnergy.asDouble - newEnergy};
    }
}

