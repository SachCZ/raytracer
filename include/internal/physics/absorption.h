#ifndef RAYTRACER_ABSORPTION_H
#define RAYTRACER_ABSORPTION_H

#include <geometry.h>
#include "magnitudes.h"
#include "gradient.h"
#include "refraction.h"
#include "laser.h"

namespace raytracer {
    /**
     * Absorption model is something that is capable of estimating power change based on intersections
     */
    class PowerExchangeModel {
    public:
        /**
         * Based on the intersections, calculate how much
         * power was absorbed in this specific element.
         * @param previousIntersection
         * @param currentIntersection
         * @param currentPower
         * @return
         */
        virtual Power getPowerChange(
                const Intersection &previousIntersection,
                const Intersection &currentIntersection,
                const Power &currentPower
        ) const = 0;

        /**
         * Name the model.
         * @return
         */
        virtual std::string getName() const = 0;
    };

    /**
     * Power exchange model estimating power lost due to laser gain
     */
    struct XRayGain : public raytracer::PowerExchangeModel {

        /** Construct providing the Gain MeshFunction */
        explicit XRayGain(const raytracer::MeshFunc &gain);

        /**
         * Based of gain coefficient estimate the amount of power exchanged
         * @param previousIntersection
         * @param currentIntersection
         * @param currentPower
         * @return power gained by plasma (power lost by plasma is negative)
         */
        raytracer::Power getPowerChange(const raytracer::Intersection &previousIntersection,
                                          const raytracer::Intersection &currentIntersection,
                                          const raytracer::Power &currentPower) const override;

        /**
         * returns "X-ray gain"
         * @return
         */
        std::string getName() const override;

    private:
        const raytracer::MeshFunc &gain;
    };

    /**
     * Model of power exchange due to resonance.
     */
    class Resonance : public PowerExchangeModel {
    public:

        /**
         * Construct this using a Gradient, a laser wavelength and a
         * Marker that marks elements in which the ray was reflected.
         * @param gradientCalculator
         * @param wavelength
         * @param reflectedMarker
         */
        Resonance(
                const Length &wavelength,
                const Marker *reflectedMarker
        );

        Resonance() = default;

        void setGradCalc(const Gradient& gradient){
            gradientCalculator = gradient;
        }

        /**
         * Get the power absorbed into the single element based on resonance absorption model from Velechovsky thesis.
         * @param previousIntersection
         * @param currentIntersection
         * @param currentPower
         * @return power change
         */
        Power getPowerChange(const Intersection &previousIntersection, const Intersection &currentIntersection,
                               const Power &currentPower) const override;

        /**
         * Returns "Resonance"
         * @return
         */
        std::string getName() const override;

    private:
        Gradient gradientCalculator;
        Length wavelength{};
        const Marker* reflectedMarker{};

        bool isResonating(const PointOnFace &pointOnFace) const;

        double getQ(Vector dir, Vector grad) const;
    };

    struct ZeroExchange : public PowerExchangeModel {
        Power getPowerChange(const Intersection &, const Intersection &, const Power &) const override {
            return Power{0};
        }

        std::string getName() const override {
            return "Zero exchange";
        }
    };

    /**
     * Absorption model of power exchange due to bremsstrahlung.
     */
    struct Bremsstrahlung : public PowerExchangeModel {

        /**
         * Provide the required functions and models to the Bremsstrahlung model to construct it.
         * @param bremssCoeff
         */
        explicit Bremsstrahlung(const MeshFunc *bremssCoeff);

        Bremsstrahlung() = default;

        /**
         * Returns the power absorbed into one element between two intersections based on bremsstrahlung model.
         * @param previousIntersection
         * @param currentIntersection
         * @param currentPower
         * @return
         */
        Power getPowerChange(
                const Intersection &previousIntersection,
                const Intersection &currentIntersection,
                const Power &currentPower
        ) const override;

        /**
         * @return "Bremsstrahlung"
         */
        std::string getName() const override;

    private:
        const MeshFunc *bremssCoeff{};
    };

    /** Map of PowerExchangeModel pointers to powers */
    typedef std::map<const PowerExchangeModel *, Power> ModelPowers;

    /** Map of PowerExchangeModel pointers to PowersSets.
     * This means ModelPowersSets[&myModel][1][2] is the power
     * absorbed by myModel in ray number 1 intersection number 2 */
    typedef std::map<const PowerExchangeModel *, PowersSet> ModelPowersSets;

    /** Summary of model-powers map and initialPower value. */
    struct AbsorptionSummary {
        /** Map of models to amount of power absorbed by the model. */
        ModelPowers modelPowers{};
        /** Total initial power of the laser obtained as a sum of ray powers.*/
        Power initialPower;
    };

    /** Class aggregating all instances of AbsorptionModel used to update absorbedPower meshFunction.*/
    class PowerExchangeController {
    public:

        /**
         * Add an AbsorptionModel that will be used when calling absorb().
         * @param model
         */
        void addModel(const PowerExchangeModel *model);

        size_t getModelsCount() const;

        ModelPowersSets genPowers(const IntersectionSet &intersectionSet, const Powers &initialPowers) const;

    private:
        std::vector<const PowerExchangeModel *> models{};

    };

    PowersSet modelPowersToRayPowers(const ModelPowersSets &modelPowersSets, const Powers& initialPowers);

    void absorbRayPowers(
            MeshFunc &absorbedPower,
            const PowersSet &powersSets,
            const IntersectionSet &intersectionSet
    );

    std::ostream& modelPowersToMsgpack(const ModelPowersSets& modelPowersSets, std::ostream& os);

    std::ostream& rayPowersToMsgpack(const PowersSet & powersSet, std::ostream& os);

    /**
     * Take absorption summary and make it a human readable string
     * @param summary
     * @return
     */
    std::string stringifyAbsorptionSummary(const AbsorptionSummary &summary);
}

#endif //RAYTRACER_ABSORPTION_H
