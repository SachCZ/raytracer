#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include <mocks.h>

using namespace testing;
using namespace raytracer;

class ModelsTest : public Test {
public:
    ModelsTest() {}

    double density{6.447e20};
    Length wavelength{1315e-7};
    Element element{0, {}, {}};
};

TEST_F(ModelsTest, critical_density_calculation_works){
    ASSERT_THAT(calcCritDens(wavelength).asDouble, DoubleNear(6.447e20, 1e17));
}

TEST_F(ModelsTest, rerfactive_index_calculation_works){
    ASSERT_THAT(calcRefractIndex(density, wavelength, 0), DoubleNear(0, 1e17));
}

TEST_F(ModelsTest, permitivity_calculation_works){
    auto result = impl::calcPermittivity(density, wavelength, 0);
    EXPECT_THAT(result.real(), DoubleNear(0, 1e17));
    ASSERT_THAT(result.imag(), DoubleNear(0, 1e17));
}

TEST_F(ModelsTest, calcSpitzerFreq_return_currect_result){
    double temperature(200.0);
    double ionization(7.0);
    ASSERT_THAT(calcSpitzerFreq(density, temperature, ionization, wavelength), DoubleEq(10201370724021.969));
}

TEST_F(ModelsTest, inverse_bremsstrahlung_coefficient_calculation_works){
    double frequency{1e17};
    ASSERT_THAT(calcInvBremssCoeff(density, wavelength, frequency), DoubleEq(684.32819033861733));
}

