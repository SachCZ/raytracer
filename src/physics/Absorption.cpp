#include "Absorption.h"

namespace raytracer {
    void AbsorptionController::addModel(const AbsorptionModel *model) {
        models.emplace_back(model);
    }

    void AbsorptionController::absorb(const Laser &laser, MeshFunction &absorbedEnergy) {
        for (const auto &laserRay : laser.getRays()) {
            this->absorbLaserRay(laserRay, absorbedEnergy);
        }
    }

    void AbsorptionController::absorbLaserRay(const LaserRay &laserRay, MeshFunction &absorbedEnergy) {
        const auto &intersections = laserRay.intersections;
        auto intersectionIt = std::next(std::begin(intersections));
        auto previousIntersectionIt = std::begin(intersections);

        auto currentEnergy = laserRay.energy.asDouble;

        for (; intersectionIt != std::end(intersections); ++intersectionIt, ++previousIntersectionIt) {
            for (const auto &model : this->models) {
                auto absorbed = model->getEnergyChange(
                        *previousIntersectionIt,
                        *intersectionIt,
                        Energy{currentEnergy},
                        laserRay
                ).asDouble;
                currentEnergy -= absorbed;
                absorbedEnergy.addValue(*(intersectionIt->previousElement), absorbed);
            }
        }
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

    bool Resonance::isResonating(const Element &element, const LaserRay& laserRay) const {
        return reflectedMarker.isMarked(element, laserRay);
    }

    double Resonance::getQ(const LaserRay &laserRay, Vector dir, Vector grad) {
        auto dir_norm = dir.getNorm();
        if(dir_norm == 0) return 0;
        auto grad_norm = grad.getNorm();
        auto lamb = laserRay.wavelength.asDouble;
        auto ne_crit = laserRay.getCriticalDensity().asDouble;
        auto sin2phi = 1 - std::pow(grad * dir / grad_norm / dir_norm, 2);
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
        const auto exponent = -laserRay.getInverseBremsstrahlungCoeff(density, frequency) * distance;

        auto newEnergy = currentEnergy.asDouble * std::exp(exponent);
        return Energy{currentEnergy.asDouble - newEnergy};
    }
}

