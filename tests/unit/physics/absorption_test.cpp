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

    Power getPowerChange(const tl::optional<Intersection> &, const Intersection &,
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
                {ContinueStraight()},
                intersectStraight,
                dontStop
        );
    }

    PowerExchangeController controller;
    MockModel mockModel;
    std::vector<double> mockAbsorbedPower = {5.0};
    Laser laser{
            Length{1356e-7},
            [](Point) { return Vector(1, 1); },
            [](double) { return 1 / 0.28284271247461912; },
            Point(-1.1, -0.9),
            Point(-0.9, -1.1),
            1
    };
    MfemMesh mesh{SegmentedLine{0.0, 1.0, 1}, SegmentedLine{0.0, 1.0, 1}};
    IntersectionSet intersections;
};

TEST_F(AbsorptionTest, addModelPowers_adds_power_to_correct_elements) {
    ModelPowersSets modelPowers = {{&mockModel, {{{0}, {3.2}}}}};

    auto rayPowers = modelPowersToRayPowers(modelPowers, {{20}});
    absorbRayPowers(mockAbsorbedPower, rayPowers, intersections);

    ASSERT_THAT(mockAbsorbedPower[0], DoubleEq(8.2));
}

TEST_F(AbsorptionTest, absorption_using_resonance_model_works) {
    ConstantGradient gradient{Vector{6.3e25, 0}};
    Marker reflectedMarker;
    reflectedMarker.mark(intersections[0][1].pointOnFace);
    Resonance resonance(laser.wavelength, &reflectedMarker, &gradient);

    auto initialPowers = generateInitialPowers(laser);

    controller.addModel(&resonance);
    auto modelPowers = controller.genPowers(intersections, initialPowers);
    auto rayPowers = modelPowersToRayPowers(modelPowers, initialPowers);
    absorbRayPowers(mockAbsorbedPower, rayPowers, intersections);

    ASSERT_THAT(mockAbsorbedPower[0], DoubleNear(5.48133, 1e-5));
}


TEST_F(AbsorptionTest, controller_generates_absorbed_powers_for_models) {
    MockModel anotherModel;
    controller.addModel(&anotherModel);
    controller.addModel(&mockModel);
    auto modelPowers = controller.genPowers(intersections, {{20}});

    ASSERT_THAT(modelPowers[&mockModel], SizeIs(1));
    ASSERT_THAT(modelPowers[&mockModel][0], SizeIs(2));
    EXPECT_THAT(modelPowers[&mockModel][0][0].asDouble, DoubleEq(11.2));
    EXPECT_THAT(modelPowers[&mockModel][0][1].asDouble, DoubleEq(11.2));

    ASSERT_THAT(modelPowers[&anotherModel], SizeIs(1));
    ASSERT_THAT(modelPowers[&anotherModel][0], SizeIs(2));
    EXPECT_THAT(modelPowers[&anotherModel][0][0].asDouble, DoubleEq(11.2));
    EXPECT_THAT(modelPowers[&anotherModel][0][1].asDouble, DoubleEq(11.2));
}

TEST(BremssTest, bremsstrahlung_power_change_is_calculated_properly) {
    std::vector<double> bremssCoeff = {2.0};
    Bremsstrahlung<decltype(bremssCoeff)> bremsstrahlung{bremssCoeff};
    Intersection prevInters;
    Intersection currentInters;
    prevInters.pointOnFace = PointOnFace{Point(0, 0), nullptr, 0};
    currentInters.pointOnFace = PointOnFace{Point(3, 0), nullptr, 1};
    Element element{0, {}, {}};
    currentInters.previousElement = &element;
    auto result = bremsstrahlung.getPowerChange(prevInters, currentInters, Power{5});
    ASSERT_THAT(result.asDouble, DoubleEq(5 * (1 - std::exp(-2 * 3))));
}

//TODO this is actually the same as bremsstrahlung coeff - should be one thing
TEST(GainTest, gain_power_change_is_calculated_properly) {
    std::vector<double> gainCoeff = {2.0};
    XRayGain<decltype(gainCoeff)> gain{gainCoeff};
    Intersection prevInters;
    Intersection currentInters;
    prevInters.pointOnFace = PointOnFace{Point(0, 0), nullptr, 0};
    currentInters.pointOnFace = PointOnFace{Point(3, 0), nullptr, 1};
    Element element{0, {}, {}};
    currentInters.previousElement = &element;
    auto result = gain.getPowerChange(prevInters, currentInters, Power{5});
    ASSERT_THAT(result.asDouble, DoubleEq(5 * (1 - std::exp(2 * 3))));
}


class FresnelTest : public Test {

public:
    void SetUp() override {
        intersection.pointOnFace = PointOnFace{Point{0, 0.5}, &face, 0};
        intersection.direction = {1, 1};
        intersection.nextElement = &element;
        marker.mark(intersection.pointOnFace);
    }

    Marker marker;
    std::vector<double> refractionIndex = {4.0};
    FresnelModel<decltype(refractionIndex)> fresnel{refractionIndex, &marker};
    Intersection intersection;
    Point a{0, 1};
    Point b{0, 0};
    Face face{
            0,
            {&a, &b}
    };
    Element element{0, {}, {}};
    double m = std::sqrt(31.0 / 32.0);
    double n = 2 * std::sqrt(2.0);
    double expected = (1 - std::pow((m - n) / (m + n), 2)) * 100;
};

TEST_F(FresnelTest, fresnel_power_change_is_calculated_properly) {
    auto res = fresnel.getPowerChange(Intersection{}, intersection, Power{100});

    ASSERT_THAT(res.asDouble, DoubleEq(expected));
}

TEST_F(FresnelTest, fresnel_power_change_is_same_for_s_and_p_in_case_of_normal_incidence) {
    FresnelModel<decltype(refractionIndex)> fresnelS{refractionIndex, &marker, "s"};
    intersection.direction = {1, 0};
    auto resP = fresnel.getPowerChange(Intersection{}, intersection, Power{100});
    auto resS = fresnelS.getPowerChange(Intersection{}, intersection, Power{100});

    ASSERT_THAT(resP.asDouble, DoubleEq(resS.asDouble));
}

TEST_F(FresnelTest, fresnel_absorbed_energy_is_correctly_processed) {
    IntersectionSet intersections{{intersection}};
    PowerExchangeController exchange;
    exchange.addModel(&fresnel);
    Powers initialPowers{Power{100}};
    auto modelPowers = exchange.genPowers(intersections, initialPowers);
    auto rayPowers = modelPowersToRayPowers(modelPowers, initialPowers);
    std::vector<double> absorbedPower = {0.0};
    absorbRayPowers(absorbedPower, rayPowers, intersections);
    ASSERT_THAT(absorbedPower[element.getId()], DoubleEq(expected));
}