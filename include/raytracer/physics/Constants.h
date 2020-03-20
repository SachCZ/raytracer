#ifndef RAYTRACER_PHYSICS_CONSTANTS_H
#define RAYTRACER_PHYSICS_CONSTANTS_H

namespace raytracer {
    /**
     * Basic physics constants in cgs units.
     */
    namespace constants {

        /** Electron charge in cgs */
        constexpr double electron_charge = 4.8032068e-10;
        /** Electron mass in cgs */
        constexpr double electron_mass = 9.1093897e-28;
        /** Speed of light in cgs */
        constexpr double speed_of_light = 2.99792458e10;
        /** Proton mass in cgs */
        constexpr double proton_mass = 1.6605e-24;
        /** Boltzmann constant in cgs */
        constexpr double boltzmann_constant = 1.602115e-12;
        /** Planck constant in cgs */
        constexpr double planck_constant = 6.626068760e-27;
        /** Reduced Planck constant in cgs */
        constexpr double reduced_planck_constant = 1.054571596e-27;
        /** Atomic mass unit in cgs */
        constexpr double atomic_mass_unit = 1.6605e-24;
    }

}

#endif //RAYTRACER_PHYSICS_CONSTANTS_H
