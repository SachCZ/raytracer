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
    Element element{0, {}};
};

TEST_F(ModelsTest, critical_density_calculation_works){
    ASSERT_THAT(calcCritDens(wavelength).asDouble, DoubleNear(6.447e20, 1e17));
}

TEST_F(ModelsTest, rerfactive_index_calculation_works){
    auto result = calcRefractiveIndex(density, wavelength);
    ASSERT_THAT(result->getValue(element), DoubleNear(0, 1e17));
}

TEST_F(ModelsTest, permitivity_calculation_works){
    auto result = impl::calcPermittivity(density.getValue(element), wavelength, 0);
    EXPECT_THAT(result.real(), DoubleNear(0, 1e17));
    ASSERT_THAT(result.imag(), DoubleNear(0, 1e17));
}

TEST_F(ModelsTest, calcSpitzerFreq_return_currect_result){
    MeshFunctionMock temperature(200.0);
    MeshFunctionMock ionization(7.0);
    auto result = calcSpitzerFreq(density, temperature, ionization, wavelength);
    ASSERT_THAT(result->getValue(element), DoubleEq(10201370724021.969));
}

TEST_F(ModelsTest, inverse_bremsstrahlung_coefficient_calculation_works){
    MeshFunctionMock frequency{1e17};
    auto result = calcInvBremssCoeff(density, wavelength, &frequency);
    ASSERT_THAT(result->getValue(element), DoubleEq(684.32819033861733));
}

