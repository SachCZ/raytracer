#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer.h>
#include <cmath>
#include <matchers.h>

using namespace testing;

TEST(AbsorptionTest, runs_as_expected) {
    using namespace raytracer;

    MfemMesh mesh("data/mesh.vtk");
    MfemL20Space space{mesh};
    MfemMeshFunction density(space, [](Point point) {return 6.44e+20 * (1 - point.x * point.x);});

    LinInterGrad gradient(calcHousGrad(mesh, density));
    Length wavelength{1315e-7};
    MfemMeshFunction refractIndex(space, [&](const Element& e){
        return calcRefractIndex(density.getValue(e), wavelength, 0);
    });
    Marker marker;
    SnellsLaw snellsLaw(&refractIndex, &marker, nullptr);
    snellsLaw.setGradCalc(gradient);
    std::vector<Ray> initDirs = {Ray{{-1.1, 0.01}, Vector{1, 0.1}}};

    auto intersectionSet = findIntersections(mesh, initDirs, snellsLaw, intersectStraight, dontStop);

    MfemMeshFunction bremsCoeff(space, [&](const Element& element) {
        auto collFreq = calcSpitzerFreq(density.getValue(element), 300, 13, wavelength);
        return calcInvBremssCoeff(density.getValue(element), wavelength, collFreq);
    });

    Bremsstrahlung bremsstrahlung(&bremsCoeff);
    Resonance resonance(wavelength, &marker);
    resonance.setGradCalc(gradient);
    FresnelModel fresnel(&refractIndex);

    PowerExchangeController exchange;
    exchange.addModel(&bremsstrahlung);
    exchange.addModel(&resonance);
    exchange.setSurfReflModel(&fresnel);

    Powers initialPowers{Power{100}};
    auto modelPowers = exchange.genPowers(intersectionSet, initialPowers);
    auto rayPowers = modelPowersToRayPowers(modelPowers, initialPowers);
    MfemMeshFunction absPower(space);
    absorbRayPowers(absPower, rayPowers, intersectionSet);
    double sum = 0;

    for (auto&& element : mesh.getElements()) {
        sum += absPower.getValue(*element);
    }

    ASSERT_THAT(sum, DoubleNear(100.0, 1e-5));
}