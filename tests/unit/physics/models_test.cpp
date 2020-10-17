#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/geometry/geometry_primitives.h>
#include <raytracer/physics/magnitudes.h>
#include <raytracer/physics/refraction.h>

using namespace testing;
using namespace raytracer;

class ModelsTest : public Test {
public:
    ColdPlasma coldPlasma{};
    ClassicCriticalDensity criticalDensity{};
    Length wavelength{1315e-7};
    Density density{6.447e20};
};

TEST_F(ModelsTest, critical_density_calculation_works){
    ASSERT_THAT(criticalDensity.getCriticalDensity(wavelength).asDouble, DoubleNear(6.447e20, 1e17));
}

TEST_F(ModelsTest, rerfactive_index_calculation_works){
    ASSERT_THAT(coldPlasma.getRefractiveIndex(density, Frequency{0}, wavelength), DoubleNear(0, 1e17));
}

TEST_F(ModelsTest, permitivity_calculation_works){
    auto result = ColdPlasma::getPermittivity(density, Frequency{0}, wavelength);
    EXPECT_THAT(result.real(), DoubleNear(0, 1e17));
    ASSERT_THAT(result.imag(), DoubleNear(0, 1e17));
}

TEST_F(ModelsTest, inverse_bremsstrahlung_coefficient_calculation_works){
    auto result = coldPlasma.getInverseBremsstrahlungCoeff(density, Frequency{1e17}, wavelength);
    ASSERT_THAT(result, DoubleEq(684.32819033861733));
}

