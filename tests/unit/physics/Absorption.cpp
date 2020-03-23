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
}

