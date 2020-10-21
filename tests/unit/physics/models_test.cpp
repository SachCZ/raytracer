#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include <mocks.h>

using namespace testing;
using namespace raytracer;

class ModelsTest : public Test {
public:
    ModelsTest() {}

    MeshFunctionMock density{6.447e20};
    Length wavelength{1315e-7};
    ColdPlasma coldPlasma{density, wavelength};
    ClassicCriticalDensity criticalDensity{};
    Element element{0, {}};
};

TEST_F(ModelsTest, critical_density_calculation_works){
    ASSERT_THAT(criticalDensity.getCriticalDensity(wavelength).asDouble, DoubleNear(6.447e20, 1e17));
}

TEST_F(ModelsTest, rerfactive_index_calculation_works){
    ASSERT_THAT(coldPlasma.getRefractiveIndex(element), DoubleNear(0, 1e17));
}

TEST_F(ModelsTest, permitivity_calculation_works){
    auto result = coldPlasma.getPermittivity(element);
    EXPECT_THAT(result.real(), DoubleNear(0, 1e17));
    ASSERT_THAT(result.imag(), DoubleNear(0, 1e17));
}

TEST_F(ModelsTest, inverse_bremsstrahlung_coefficient_calculation_works){
    MeshFunctionMock frequency{1e17};
    ColdPlasma coldPlasmaWithBremsstrahlung{density, wavelength, &frequency};
    auto result = coldPlasmaWithBremsstrahlung.getInverseBremsstrahlungCoeff(element);
    ASSERT_THAT(result, DoubleEq(684.32819033861733));
}

