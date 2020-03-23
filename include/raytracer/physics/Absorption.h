#ifndef RAYTRACER_ABSORPTION_H
#define RAYTRACER_ABSORPTION_H

#include "Ray.h"
#include "MeshFunction.h"
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
        void addModel(const AbsorptionModel *model);

        /**
         * Add energy values based on AbsorptionModel applied on the Laser to the absorbedEnergy
         * MeshFunction.
         *
         * @warning For this to work properly, Laser::generateIntersections() must be called first.
         * @param laser
         * @param absorbedEnergy
         */
        void absorb(const Laser &laser, MeshFunction &absorbedEnergy);

    private:
        std::vector<const AbsorptionModel *> models{};

        void absorbLaserRay(const LaserRay &laserRay, MeshFunction &absorbedEnergy);
    };
    /**
     * @}
     */
}

#endif //RAYTRACER_ABSORPTION_H
