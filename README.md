# Raytracer
A c++ code written for a diploma thesis at [FNSPE CTU](https://www.fjfi.cvut.cz/en/).
The goal is to simulate a laser-plasma interaction using the geometric approximation.
The main focus is on usage with hydrodynamic simulations. Specifically simulations of plasma
used as a gain medium for x-ray laser generation. Using RayTracer both the plasma generation
using a driving laser and x-ray lasing can be simulated. 

## Reading the docs
[//]: # (TODO expand this when the docs are coplete)

## Building and installing

### Dependencies
The project depends on multiple libraries. These are divided into two categories: small
libraries which are build during cmake configure and large libraries which need to be
installed by the user.

The small libraries are:
[Google Test](https://github.com/google/googletest),
[msgpack](https://github.com/msgpack/msgpack-c/tree/cpp_master) and
[jsoncpp](https://github.com/open-source-parsers/jsoncpp). As previously mentioned,
these will be built during cmake configure and no further action is required from
the user.

The project also depends on large libraries, namely:
[Boost headers](https://www.boost.org/) and
[mfem](https://mfem.org/).

These must be provided by the user, meaning they must be installed at the users system
in a location available to cmake (see [Build using cmake](Build using cmake)).

On ubuntu based systems (and similarly on other systems) you can install boost using
```$bash
apt-get install libboost-all-dev
```

Mfem must be installed manually. The basic commands to install mfem are:
```bash
git clone https://github.com/mfem/mfem &&
mkdir mfem_build &&
cd mfem_build &&
cmake ../mfem &&
make install
```

### Build using cmake




Link to [documentation](https://sachcz.github.io/raytracer).
