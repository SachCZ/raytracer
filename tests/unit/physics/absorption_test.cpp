#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include "../../support/mocks.h"


using namespace testing;
using namespace raytracer;


class MockModel : public PowerExchangeModel {
public:
    std::string getName() const override {
        return "Mock model";
    }

    Power getPowerChange(const Intersection &, const Intersection &,
                           const Power &) const override {
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
    }

    PowerExchangeController controller;
    MockModel mockModel;
    MeshFunctionMock mockAbsorbedPower;
    Laser laser{
            Length{1356e-7},
            [](Point) { return Vector(1, 1); },
            [](double) { return 1/0.28284271247461912; },
            Point(-1.1, -0.9),
            Point(-0.9, -1.1),
            1
    };
    MfemMesh mesh{SegmentedLine{0.0, 1.0, 1}, SegmentedLine{0.0, 1.0, 1}};
    IntersectionSet intersections;
};

TEST_F(AbsorptionTest, addModelPowers_adds_power_to_correct_elements) {
    ModelPowersSets modelPowers = {{&mockModel, {{{0}, {3.2}}}}};

    mockAbsorbedPower.setValue(Element{0, {}, {}}, 5);
    auto rayPowers = modelPowersToRayPowers(modelPowers, {{20}});
    absorbRayPowers(mockAbsorbedPower, rayPowers, intersections);

    ASSERT_THAT(mockAbsorbedPower.getValue(Element(0, {}, {})), DoubleEq(8.2));
}

TEST_F(AbsorptionTest, absorption_using_resonance_model_works) {
    ConstantGradient gradient{Vector{6.3e25, 0}};
    Marker reflectedMarker;
    reflectedMarker.mark(intersections[0][1].pointOnFace);
    Resonance resonance(laser.wavelength, &reflectedMarker);
    resonance.setGradCalc(gradient);

    auto initialPowers = generateInitialPowers(laser);

    mockAbsorbedPower.setValue(Element{0, {}, {}}, 5);
    controller.addModel(&resonance);
    auto modelPowers = controller.genPowers(intersections, initialPowers);
    auto rayPowers = modelPowersToRayPowers(modelPowers, initialPowers);
    absorbRayPowers(mockAbsorbedPower, rayPowers, intersections);

    ASSERT_THAT(mockAbsorbedPower.getValue(Element(0, {}, {})), DoubleNear(5.48133, 1e-5));
}



TEST_F(AbsorptionTest, controller_generates_absorbed_powers_for_models) {
    MockModel anotherModel;
    controller.addModel(&anotherModel);
    controller.addModel(&mockModel);
    auto modelPowers = controller.genPowers(intersections, {{20}});

    ASSERT_THAT(modelPowers[&mockModel], SizeIs(1));
    ASSERT_THAT(modelPowers[&mockModel][0], SizeIs(2));
    EXPECT_THAT(modelPowers[&mockModel][0][0].asDouble, DoubleEq(0));
    EXPECT_THAT(modelPowers[&mockModel][0][1].asDouble, DoubleEq(11.2));

    ASSERT_THAT(modelPowers[&anotherModel], SizeIs(1));
    ASSERT_THAT(modelPowers[&anotherModel][0], SizeIs(2));
    EXPECT_THAT(modelPowers[&anotherModel][0][0].asDouble, DoubleEq(0));
    EXPECT_THAT(modelPowers[&anotherModel][0][1].asDouble, DoubleEq(11.2));
}

TEST(BremssTest, bremsstrahlung_power_change_is_calculated_properly){
    MeshFunctionMock bremssCoeff(2.0);
    Bremsstrahlung bremsstrahlung{&bremssCoeff};
    Intersection prevInters;
    Intersection currentInters;
    prevInters.pointOnFace = PointOnFace{Point(0,0), nullptr, 0};
    currentInters.pointOnFace = PointOnFace{Point(3,0), nullptr, 1};
    Element element {0, {}, {}};
    currentInters.previousElement = &element;
    auto result = bremsstrahlung.getPowerChange(prevInters, currentInters, Power{5});
    ASSERT_THAT(result.asDouble, DoubleEq(5*(1 - std::exp(-2*3))));
}

//TODO this is actually the same as bremsstrahlung coeff - should be one thing
TEST(GainTest, gain_power_change_is_calculated_properly){
    MeshFunctionMock gainCoeff(2.0);
    XRayGain gain{gainCoeff};
    Intersection prevInters;
    Intersection currentInters;
    prevInters.pointOnFace = PointOnFace{Point(0,0), nullptr, 0};
    currentInters.pointOnFace = PointOnFace{Point(3,0), nullptr, 1};
    Element element {0, {}, {}};
    currentInters.previousElement = &element;
    auto result = gain.getPowerChange(prevInters, currentInters, Power{5});
    ASSERT_THAT(result.asDouble, DoubleEq(5*(1 - std::exp(2*3))));
}

