#ifndef RAYTRACER_ABSORPTION_H
#define RAYTRACER_ABSORPTION_H

#include "Ray.h"
#include "MeshFunction.h"
#include "Magnitudes.h"
#include "LaserRay.h"
#include "Laser.h"
#include "CollisionalFrequency.h"
#include "Gradient.h"
#include "Refraction.h"

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

        virtual std::string getName() const = 0;
    };

    /**
     * Absorption model of energy change due to resonance.
     */
    class Resonance : public AbsorptionModel {
    public:

        /**
         * Construct this using a Gradient and Marker that marks elements in which the ray was reflected.
         * @param gradientCalculator
         * @param reflectedMarker
         */
        Resonance(const Gradient &gradientCalculator, const Marker &reflectedMarker);

        /**
         * Get the energy absorbed into the single element based on resonance absorption model from Velechovsky thesis.
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
        ) const override;

        std::string getName() const override {
            return "Resonance";
        }

    private:
        const Gradient &gradientCalculator;
        const Marker &reflectedMarker;

        bool isResonating(const Element &element, const LaserRay& laserRay) const;

        static double getQ(const LaserRay &laserRay, Vector dir, Vector grad);
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
        );

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
        ) const override;

        std::string getName() const override {
            return "Bremsstrahlung";
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
        std::map<const AbsorptionModel *, Energy> absorb(const Laser &laser, MeshFunction &absorbedEnergy);

    private:
        std::vector<const AbsorptionModel *> models{};

        std::map<const AbsorptionModel *, Energy> absorbLaserRay(const LaserRay &laserRay, MeshFunction &absorbedEnergy);
    };
    /**
     * @}
     */
}

#endif //RAYTRACER_ABSORPTION_H
