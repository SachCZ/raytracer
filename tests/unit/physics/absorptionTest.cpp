#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/physics/Absorption.h>
#include <raytracer/physics/Magnitudes.h>
#include <raytracer/physics/Refraction.h>
#include <raytracer/physics/Propagation.h>
#include <raytracer/physics/Termination.h>


using namespace testing;
using namespace raytracer;

class MockModel : public AbsorptionModel {
public:
    Energy getEnergyChange(const Intersection &previousIntersection, const Intersection &currentIntersection,
                           const Energy &currentEnergy, const LaserRay &laserRay) const override {
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

class absorption_controller : public Test {

public:
    void SetUp() override {
        DiscreteLine side{};
        side.length = 1;
        side.segmentCount = 1;
        mfemMesh = constructRectangleMesh(side, side);
        mesh = std::make_unique<Mesh>(mfemMesh.get());
        laser.generateRays(1);
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
            [](Point) { return Vector(1, 0); },
            [](double) { return 1; },
            Point(-1, 0),
            Point(-1, 1)
    };
    std::unique_ptr<mfem::Mesh> mfemMesh;
    std::unique_ptr<Mesh> mesh;
};

TEST_F(absorption_controller, does_absorb_energy_according_to_model) {
    controller.addModel(&mockModel);
    controller.absorb(laser, mockAbsorbedEnergy);

    auto newEnergy = mockAbsorbedEnergy.getValue(Element(0, {}));
    ASSERT_THAT(newEnergy, DoubleEq(16.2));
}