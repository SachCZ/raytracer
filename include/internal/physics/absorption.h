#ifndef RAYTRACER_ABSORPTION_H
#define RAYTRACER_ABSORPTION_H

#include <geometry.h>

#include <utility>
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
                const tl::optional<Intersection> &previousIntersection,
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
    template<typename MeshFunc>
    struct XRayGain : public raytracer::PowerExchangeModel {

        /** Construct providing the Gain MeshFunction */
        explicit XRayGain(const MeshFunc &gain) : gain(gain) {}

        /**
         * Based of gain coefficient estimate the amount of power exchanged
         * @param previousIntersection
         * @param currentIntersection
         * @param currentPower
         * @return power gained by plasma (power lost by plasma is negative)
         */
        raytracer::Power getPowerChange(
                const tl::optional<Intersection> &previousIntersection,
                const raytracer::Intersection &currentIntersection,
                const raytracer::Power &currentPower) const override {
            if (!previousIntersection) return {0};
            auto distance = (currentIntersection.pointOnFace.point -
                             previousIntersection.value().pointOnFace.point).getNorm();
            auto element = currentIntersection.previousElement;
            auto gainCoeff = gain[element->getId()];
            return raytracer::Power{currentPower.asDouble * (1 - std::exp(gainCoeff * distance))};
        }

        /**
         * returns "X-ray gain"
         * @return
         */
        std::string getName() const override {
            return "X-ray gain";
        }

    private:
        const MeshFunc gain;
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
                const Marker *reflectedMarker,
                const Gradient *gradCalc
        );

        Resonance() = default;

        /**
         * Get the power absorbed into the single element based on resonance absorption model from Velechovsky thesis.
         * @param previousIntersection
         * @param currentIntersection
         * @param currentPower
         * @return power change
         */
        Power
        getPowerChange(const tl::optional<Intersection> &previousIntersection, const Intersection &currentIntersection,
                       const Power &currentPower) const override;

        /**
         * Returns "Resonance"
         * @return
         */
        std::string getName() const override;

    private:
        Length wavelength{};
        const Marker *reflectedMarker{};
        const Gradient *gradCalc{};

        bool isResonating(const PointOnFace &pointOnFace) const;

        double getQ(Vector dir, Vector grad) const;
    };

    struct ZeroExchange : public PowerExchangeModel {
        Power getPowerChange(const tl::optional<Intersection> &, const Intersection &, const Power &) const override {
            return Power{0};
        }

        std::string getName() const override {
            return "Zero exchange";
        }
    };

    template<typename MeshFunc>
    class FresnelModel : public PowerExchangeModel {
    public:
        FresnelModel() = default;

        explicit FresnelModel(const MeshFunc &refractIndex, const Marker *reflectedMarker,
                              std::string polarization = "p") :
                refractIndex(refractIndex), reflectedMarker(reflectedMarker),
                polarization(std::move(polarization)) {}

        Power getPowerChange(const tl::optional<Intersection> &, const Intersection &currentIntersection,
                             const Power &currentPower) const override {
            if (reflectedMarker->isMarked(currentIntersection.pointOnFace)) {
                if (!currentIntersection.nextElement) {
                    return {0};
                }
                double n2 = refractIndex[currentIntersection.nextElement->getId()];
                if (n2 <= 0) {
                    return Power{0};
                }
                double n1 = 1.0;
                auto normal = currentIntersection.pointOnFace.face->getNormal();
                normal = 1 / normal.getNorm() * normal;
                auto dir = currentIntersection.direction;
                dir = 1 / dir.getNorm() * dir;
                auto cosInc = std::abs(dir * normal);
                if (polarization == "s") {
                    return {(1 - Rs(n1, n2, cosInc)) * currentPower.asDouble};
                } else {
                    return {(1 - Rp(n1, n2, cosInc)) * currentPower.asDouble};
                }
            } else {
                return {0};
            }
        }

    private:
        static double Rs(double n1, double n2, double cosInc) {
            auto sin2 = 1 - cosInc * cosInc;
            auto a = n1 * cosInc;
            auto root = 1 - std::pow(n1 / n2, 2) * sin2;
            auto b = n2 * std::sqrt(std::max(root, 0.0));
            return std::pow((a - b) / (a + b), 2);
        }

        static double Rp(double n1, double n2, double cosInc) {
            auto sin2 = 1 - cosInc * cosInc;
            auto root = 1 - std::pow(n1 / n2, 2) * sin2;
            auto a = n1 * std::sqrt(std::max(root, 0.0));
            auto b = n2 * cosInc;
            return std::pow((a - b) / (a + b), 2);
        }

        std::string getName() const override {
            return "Fresnel";
        }

        const MeshFunc refractIndex{};
        const Marker *reflectedMarker{};
        std::string polarization;
    };

    /**
     * Absorption model of power exchange due to bremsstrahlung.
     */
    template<typename MeshFunc>
    struct Bremsstrahlung : public PowerExchangeModel {

        /**
         * Provide the required functions and models to the Bremsstrahlung model to construct it.
         * @param bremssCoeff
         */
        explicit Bremsstrahlung(const MeshFunc &bremssCoeff) : bremssCoeff(bremssCoeff) {}

        Bremsstrahlung() = default;

        /**
         * Returns the power absorbed into one element between two intersections based on bremsstrahlung model.
         * @param previousIntersection
         * @param currentIntersection
         * @param currentPower
         * @return
         */
        Power getPowerChange(
                const tl::optional<Intersection> &previousIntersection,
                const Intersection &currentIntersection,
                const Power &currentPower
        ) const override {
            if (!previousIntersection) return {0};
            const auto &element = currentIntersection.previousElement;
            if (!element) return Power{0};
            const auto &previousPoint = previousIntersection.value().pointOnFace.point;
            const auto &point = currentIntersection.pointOnFace.point;

            const auto distance = (point - previousPoint).getNorm();
            auto coeff = bremssCoeff[element->getId()];
            const auto exponent = -coeff * distance;

            auto newPower = currentPower.asDouble * std::exp(exponent);
            return Power{currentPower.asDouble - newPower};
        }

        /**
         * @return "Bremsstrahlung"
         */
        std::string getName() const override {
            return "Bremsstrahlung";
        }

    private:
        const MeshFunc bremssCoeff{};
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

        std::vector<const PowerExchangeModel *> models{};
    };

    PowersSet modelPowersToRayPowers(const ModelPowersSets &modelPowersSets, const Powers &initialPowers);

    std::vector<double> absorbRayPowers(
            std::size_t elementsCount,
            const PowersSet &powersSets,
            const IntersectionSet &intersectionSet
    );

    std::ostream &modelPowersToMsgpack(const ModelPowersSets &modelPowersSets, std::ostream &os);

    std::ostream &rayPowersToMsgpack(const PowersSet &powersSet, std::ostream &os);

    /**
     * Take absorption summary and make it a human readable string
     * @param summary
     * @return
     */
    std::string stringifyAbsorptionSummary(const AbsorptionSummary &summary);
}

#endif //RAYTRACER_ABSORPTION_H
