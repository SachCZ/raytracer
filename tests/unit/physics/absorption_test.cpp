#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include "../../support/mocks.h"


using namespace testing;
using namespace raytracer;


class MockModel : public EnergyExchangeModel {
public:
    std::string getName() const override {
        return "Mock model";
    }

    Energy getEnergyChange(const Intersection &, const Intersection &,
                           const Energy &) const override {
        return {11.2};
    }
};

class AbsorptionTest : public Test {

public:
    void SetUp() override {
        intersections = findIntersections(
                mesh,
                generateInitialDirections(laser),
                ContinueStraight(),
                intersectStraight,
                dontStop
        );
        initialEnergies = generateInitialEnergies(laser);
    }

    EnergyExchangeController controller;
    MockModel mockModel;
    MeshFunctionMock mockAbsorbedEnergy;
    Laser laser{
            Length{1356e-7},
            [](Point) { return Vector(1, 1); },
            [](double) { return 1/0.28284271247461912; },
            Point(-1.1, -0.9),
            Point(-0.9, -1.1),
            1
    };
    MfemMesh mesh{SegmentedLine{1.0, 1}, SegmentedLine{1.0, 1}};
    IntersectionSet intersections;
    Energies initialEnergies;
};

TEST_F(AbsorptionTest, controller_does_absorb_energy_according_to_model) {
    mockAbsorbedEnergy.setValue(Element{0, {}}, 5);
    controller.addModel(&mockModel);
    controller.absorb(intersections, initialEnergies, mockAbsorbedEnergy);

    ASSERT_THAT(mockAbsorbedEnergy.getValue(Element(0, {})), DoubleEq(16.2));
}

TEST_F(AbsorptionTest, absorption_using_resonance_model_works) {
    ConstantGradient gradient{Vector{6.3e25, 0}};
    Marker reflectedMarker;
    reflectedMarker.mark(*intersections[0][1].previousElement, intersections[0][1].pointOnFace);
    Resonance resonance(gradient, laser.wavelength, reflectedMarker);

    mockAbsorbedEnergy.setValue(Element{0, {}}, 5);
    controller.addModel(&resonance);
    controller.absorb(intersections, initialEnergies, mockAbsorbedEnergy);

    ASSERT_THAT(mockAbsorbedEnergy.getValue(Element(0, {})), DoubleNear(5.48133, 1e-5));
}

TEST(BremssTest, bremsstrahlung_energy_change_is_calculated_properly){
    MeshFunctionMock bremssCoeff(2.0);
    Bremsstrahlung bremsstrahlung{bremssCoeff};
    Intersection prevInters;
    Intersection currentInters;
    prevInters.pointOnFace = PointOnFace{Point(0,0), nullptr, 0};
    currentInters.pointOnFace = PointOnFace{Point(3,0), nullptr, 1};
    Element element {0, {}};
    currentInters.previousElement = &element;
    auto result = bremsstrahlung.getEnergyChange(prevInters, currentInters, Energy{5});
    ASSERT_THAT(result.asDouble, DoubleEq(5*(1 - std::exp(-2*3))));
}

//TODO this is actually the same as bremsstrahlung coeff - should be one thing
TEST(GainTest, gain_energy_change_is_calculated_properly){
    MeshFunctionMock gainCoeff(2.0);
    XRayGain gain{gainCoeff};
    Intersection prevInters;
    Intersection currentInters;
    prevInters.pointOnFace = PointOnFace{Point(0,0), nullptr, 0};
    currentInters.pointOnFace = PointOnFace{Point(3,0), nullptr, 1};
    Element element {0, {}};
    currentInters.previousElement = &element;
    auto result = gain.getEnergyChange(prevInters, currentInters, Energy{5});
    ASSERT_THAT(result.asDouble, DoubleEq(5*(1 - std::exp(2*3))));
}