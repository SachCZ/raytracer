#ifndef RAYTRACER_ABSORPTION_H
#define RAYTRACER_ABSORPTION_H

#include <raytracer/geometry/Ray.h>
#include <raytracer/geometry/MeshFunction.h>
#include "Magnitudes.h"
#include "LaserRay.h"
#include "Laser.h"

namespace raytracer {
    /**
     * \addtogroup absorption
     * @{
     */

    /**
     * Abstract interface, to obey this getEnergyChange must be implemented.
     */
    class AbsorptionModel {
    public:
        /**
         * Based on intersections of tray with the element, calculate how much
         * energy was absorbed in this specific element.
         * @param previousIntersection
         * @param currentIntersection
         * @param currentEnergy
         * @param laserRay
         * @return
         */
        virtual Energy getEnergyChange(
                const Intersection &previousIntersection,
                const Intersection &currentIntersection,
                const Energy &currentEnergy,
                const LaserRay &laserRay
        ) const = 0;
    };

    /**
     * @}
     */

    /**
     * \addtogroup api
     * @{
     */

    /**
     * Class aggregating all instances of AbsorptionModel used to update absorbedEnergy meshFunction.
     */
    class AbsorptionController {
    public:

        /**
         * Add an AbsorptionModel that will be used when calling absorb().
         * @param model
         */
        void addModel(const AbsorptionModel *model) {
            models.emplace_back(model);
        }

        /**
         * Add energy values based on AbsorptionModel applied on the Laser to the absorbedEnergy
         * MeshFunction.
         *
         * @warning For this to work properly, Laser::generateIntersections() must be called first.
         * @param laser
         * @param absorbedEnergy
         */
        void absorb(const Laser &laser, MeshFunction &absorbedEnergy) {
            for (const auto &laserRay : laser.getRays()) {
                this->absorbLaserRay(laserRay, absorbedEnergy);
            }
        }

    private:
        std::vector<const AbsorptionModel *> models{};

        void absorbLaserRay(const LaserRay &laserRay, MeshFunction &absorbedEnergy) {
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
    };
    /**
     * @}
     */
}

#endif //RAYTRACER_ABSORPTION_H
