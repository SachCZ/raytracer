#ifndef RAYTRACER_ABSORPTION_H
#define RAYTRACER_ABSORPTION_H

#include "Ray.h"
#include "MeshFunction.h"
#include "Magnitudes.h"
#include "LaserRay.h"
#include "Laser.h"
#include "CollisionalFrequency.h"

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
     * Absorption model of energy exchange due to bremsstrahlung.
     */
    struct Bremsstrahlung : public AbsorptionModel {

        /**
         * To be able to model bremsstrahlung, MeshFunction density, temperature, ionization must be provided.
         * Besides that a collisional frequency model is needed.
         * @param density
         * @param temperature
         * @param ionization
         * @param collisionalFrequency
         */
        explicit Bremsstrahlung(
                const MeshFunction &density,
                const MeshFunction &temperature,
                const MeshFunction &ionization,
                const CollisionalFrequency &collisionalFrequency
        ) :
                _density(density),
                _temperature(temperature),
                _ionization(ionization),
                collisionalFrequency(collisionalFrequency) {}

        /**
         * Returns the energy absorbed into one element between two intersections based on bremsstrahlung model.
         * @param previousIntersection
         * @param currentIntersection
         * @param currentEnergy
         * @param laserRay
         * @return
         */
        Energy getEnergyChange(
                const Intersection &previousIntersection,
                const Intersection &currentIntersection,
                const Energy &currentEnergy,
                const LaserRay &laserRay
        ) const override {
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

    private:
        const MeshFunction &_density;
        const MeshFunction &_temperature;
        const MeshFunction &_ionization;
        const CollisionalFrequency &collisionalFrequency;
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
