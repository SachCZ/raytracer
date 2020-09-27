#ifndef RAYTRACER_ABSORPTION_H
#define RAYTRACER_ABSORPTION_H

#include "Ray.h"
#include "MeshFunction.h"
#include "Magnitudes.h"
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
        virtual Energy
        getEnergyChange(const Intersection &previousIntersection, const Intersection &currentIntersection,
                        const Energy &currentEnergy) const = 0;

        virtual std::string getName() const = 0;
    };

    struct XRayGain : public raytracer::AbsorptionModel {
        explicit XRayGain(const raytracer::MeshFunction& gain);

        raytracer::Energy getEnergyChange(const raytracer::Intersection &previousIntersection,
                                          const raytracer::Intersection &currentIntersection,
                                          const raytracer::Energy &currentEnergy) const override;

        std::string getName() const override;

    private:
        const raytracer::MeshFunction& gain;
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
        Resonance(
                const Gradient &gradientCalculator,
                const CriticalDensity &criticalDensity,
                const Length &wavelength,
                const Marker &reflectedMarker
        );

        /**
         * Get the energy absorbed into the single element based on resonance absorption model from Velechovsky thesis.
         * @param previousIntersection
         * @param currentIntersection
         * @param currentEnergy
         * @param laserRay
         * @return
         */
        Energy getEnergyChange(const Intersection &previousIntersection, const Intersection &currentIntersection,
                               const Energy &currentEnergy) const override;

        std::string getName() const override {
            return "Resonance";
        }

    private:
        const Gradient &gradientCalculator;
        const CriticalDensity& criticalDensity;
        const Length &wavelength;
        const Marker &reflectedMarker;

        bool isResonating(const Element &element, const PointOnFace &pointOnFace) const;

        double getQ(Vector dir, Vector grad) const;
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
                const CollisionalFrequency &collisionalFrequency,
                const BremsstrahlungCoeff &bremsstrahlungCoeff,
                const Length &wavelength
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
                const Energy &currentEnergy
        ) const override;

        std::string getName() const override {
            return "Bremsstrahlung";
        }

    private:
        const MeshFunction &_density;
        const MeshFunction &_temperature;
        const MeshFunction &_ionization;
        const CollisionalFrequency &collisionalFrequency;
        const BremsstrahlungCoeff &bremsstrahlungCoeff;
        const Length wavelength;
    };

    /**
     * @}
     */

    /**
     * \addtogroup api
     * @{
     */

    typedef std::map<const AbsorptionModel *, Energy> ModelEnergies;
    struct AbsorptionSummary {
        ModelEnergies modelEnergies{};
        Energy initialEnergy;
    };

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
         * @param rays
         * @param absorbedEnergy
         */
        AbsorptionSummary absorb(
                const IntersectionSet &intersectionSet,
                const EnergiesSet &energiesSet,
                MeshFunction &absorbedEnergy
        );

    private:
        std::vector<const AbsorptionModel *> models{};

        ModelEnergies absorbLaserRay(const Intersections &intersections, const Energy &initialEnergy,
                                     MeshFunction &absorbedEnergy);
    };

    std::string stringifyAbsorptionSummary(const AbsorptionSummary &summary);

    /**
     * @}
     */
}

#endif //RAYTRACER_ABSORPTION_H
