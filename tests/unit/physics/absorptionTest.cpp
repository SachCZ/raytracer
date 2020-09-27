#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/physics/Absorption.h>
#include <raytracer/physics/Magnitudes.h>
#include <raytracer/physics/Refraction.h>
#include <raytracer/physics/Propagation.h>
#include <raytracer/physics/Termination.h>


using namespace testing;
using namespace raytracer;

class MockGradient : public Gradient {
public:
    Vector
    get(const PointOnFace &, const Element &, const Element &) const override {
        return {6.3e25, 0};
    }
};

class MockModel : public AbsorptionModel {
public:
    std::string getName() const override {
        return "Mock model";
    }

    Energy getEnergyChange(const Intersection &previousIntersection, const Intersection &currentIntersection,
                           const Energy &currentEnergy) const override {
        return {11.2};
    }
};

class MockAbsorbedEnergy : public MeshFunction {
public:
    double getValue(const Element &element) const override {
        return this->_value;
    }

    void setValue(const Element &element, double value) override {
        this->_value = value;
    }

    void addValue(const Element &element, double value) override {
        this->_value += value;
    }

private:
    double _value{5};
};

class absorption : public Test {

public:
    void SetUp() override {
        DiscreteLine side{};
        side.length = 1;
        side.segmentCount = 1;
        mfemMesh = constructMfemMesh(side, side);
        mesh = std::make_unique<MfemMesh>(mfemMesh.get());
        laser.generateInitialRays(<#initializer#>, 1);
        laser.generateIntersections(
                *mesh,
                ContinueStraight(),
                intersectStraight,
                DontStop()
        );
    }

    AbsorptionController controller;
    MockModel mockModel;
    MockAbsorbedEnergy mockAbsorbedEnergy;
    Laser laser{
            Length{1356e-7},
            [](Point) { return Vector(1, 1); },
            [](double) { return 1/0.28284271247461912; },
            Point(-1.1, -0.9),
            Point(-0.9, -1.1)
    };
    std::unique_ptr<mfem::Mesh> mfemMesh;
    std::unique_ptr<MfemMesh> mesh;
};

TEST_F(absorption, controller_does_absorb_energy_according_to_model) {
    controller.addModel(&mockModel);
    controller.absorb(laser, mockAbsorbedEnergy);

    auto newEnergy = mockAbsorbedEnergy.getValue(Element(0, {}));
    ASSERT_THAT(newEnergy, DoubleEq(16.2));
}

TEST_F(absorption, using_resonance_model_works) {
    MockGradient gradient;
    Marker reflectedMarker;
    reflectedMarker.mark(*laser.getRays().front().intersections[0].nextElement, laser.getRays().front());
    Resonance resonance(gradient, reflectedMarker);

    controller.addModel(&resonance);
    controller.absorb(laser, mockAbsorbedEnergy);

    auto newEnergy = mockAbsorbedEnergy.getValue(Element(0, {}));
    ASSERT_THAT(newEnergy, DoubleNear(5.48133, 1e-5));
}