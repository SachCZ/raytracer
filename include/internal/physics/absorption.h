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
         * Based on the intersections, calculate how much
         * energy was absorbed in this specific element.
         * @param previousIntersection
         * @param currentIntersection
         * @param currentEnergy
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

        /** Construct providing the Gain MeshFunction */
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
         * Construct this using a Gradient, a CriticalDensity, a laser wavelength and a
         * Marker that marks elements in which the ray was reflected.
         * @param gradientCalculator
         * @param criticalDensity
         * @param wavelength
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
         * @return energy change
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
         * Provide the required functions and models to the Bremsstrahlung model to construct it.
         * @param density
         * @param temperature
         * @param ionization
         * @param collisionalFrequency
         * @param bremsstrahlungCoeff
         * @param wavelength
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

    /** Map of EnergyExchangeModel pointers to energies */
    typedef std::map<const EnergyExchangeModel *, Energy> ModelEnergies;

    /** Summary of model-energies map and initialEnergy value. */
    struct AbsorptionSummary {
        /** Map of models to amount of energy absorbed by the model. */
        ModelEnergies modelEnergies{};
        /** Total initial energy of the laser obtained as a sum of ray energies.*/
        Energy initialEnergy;
    };

    /** Class aggregating all instances of AbsorptionModel used to update absorbedEnergy meshFunction.*/
    class EnergyExchangeController {
    public:

        /**
         * Add an AbsorptionModel that will be used when calling absorb().
         * @param model
         */
        void addModel(const EnergyExchangeModel *model);


        /**
         * Modify the absorbedEnergy MeshFunction based on the intersectionSet and initialEnergies of the rays,
         * then return a summary.
         * @param intersectionSet
         * @param initialEnergies
         * @param absorbedEnergy
         * @return
         */
        AbsorptionSummary absorb(
                const IntersectionSet &intersectionSet,
                const Energies &initialEnergies,
                MeshFunction &absorbedEnergy
        );

    private:
        std::vector<const EnergyExchangeModel *> models{};

        ModelEnergies absorbLaserRay(
                const Intersections &intersections,
                const Energy &initialEnergy,
                MeshFunction &absorbedEnergy
        );
    };

    /**
     * Take absorption summary and make it a human readable string
     * @param summary
     * @return
     */
    std::string stringifyAbsorptionSummary(const AbsorptionSummary &summary);
}

#endif //RAYTRACER_ABSORPTION_H
