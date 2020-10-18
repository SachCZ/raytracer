#ifndef RAYTRACER_ABSORPTION_H
#define RAYTRACER_ABSORPTION_H

#include <geometry.h>
#include "magnitudes.h"
#include "gradient.h"
#include "refraction.h"
#include "laser.h"

namespace raytracer {
    /**
     * Absorption model is something that is capable of estimating energy change based on intersections
     */
    class EnergyExchangeModel {
    public:
        /**
         * Based on intersections, calculate how much
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
                const Energy &currentEnergy
        ) const = 0;

        /**
         * Name the model.
         * @return
         */
        virtual std::string getName() const = 0;
    };

    /**
     * Energy exchange model estimating energy lost due to laser gain
     */
    struct XRayGain : public raytracer::EnergyExchangeModel {
        explicit XRayGain(const raytracer::MeshFunction &gain);

        /**
         * Based of gain coefficient estimate the amount of energy exchanged
         * @param previousIntersection
         * @param currentIntersection
         * @param currentEnergy
         * @return energy gained by plasma (energy lost by plasma is negative)
         */
        raytracer::Energy getEnergyChange(const raytracer::Intersection &previousIntersection,
                                          const raytracer::Intersection &currentIntersection,
                                          const raytracer::Energy &currentEnergy) const override;
        /**
         * returns "X-ray gain"
         * @return
         */
        std::string getName() const override;

    private:
        const raytracer::MeshFunction &gain;
    };

    /**
     * Model of energy exchange due to resonance.
     */
    class Resonance : public EnergyExchangeModel {
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

        /**
         * Returns "Resonance"
         * @return
         */
        std::string getName() const override;

    private:
        const Gradient &gradientCalculator;
        const CriticalDensity &criticalDensity;
        const Length &wavelength;
        const Marker &reflectedMarker;

        bool isResonating(const Element &element, const PointOnFace &pointOnFace) const;

        double getQ(Vector dir, Vector grad) const;
    };

    /**
     * Absorption model of energy exchange due to bremsstrahlung.
     */
    struct Bremsstrahlung : public EnergyExchangeModel {

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

        /**
         * @return "Bremsstrahlung"
         */
        std::string getName() const override;

    private:
        const MeshFunction &_density;
        const MeshFunction &_temperature;
        const MeshFunction &_ionization;
        const CollisionalFrequency &collisionalFrequency;
        const BremsstrahlungCoeff &bremsstrahlungCoeff;
        const Length wavelength;
    };

    typedef std::map<const EnergyExchangeModel *, Energy> ModelEnergies;
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
        void addModel(const EnergyExchangeModel *model);

        /**
         * Add energy values based on AbsorptionModel applied on the Laser to the absorbedEnergy
         * MeshFunction and return the absorption summary
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
        std::vector<const EnergyExchangeModel *> models{};

        ModelEnergies absorbLaserRay(const Intersections &intersections, const Energy &initialEnergy,
                                     MeshFunction &absorbedEnergy);
    };

    /**
     * Take absorption summary and make it a human readable string
     * @param summary
     * @return
     */
    std::string stringifyAbsorptionSummary(const AbsorptionSummary &summary);
}

#endif //RAYTRACER_ABSORPTION_H
