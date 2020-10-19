# RayTracer
A c++ code written for a diploma thesis at [FNSPE CTU](https://www.fjfi.cvut.cz/en/).
The goal is to simulate a laser-plasma interaction using the geometric approximation.
The main focus is on usage with hydrodynamic simulations. Specifically simulations of plasma
used as a gain medium for x-ray laser generation. Using RayTracer both the plasma generation
using a driving laser and x-ray lasing can be simulated. 

## Reading the docs
[//]: # (TODO expand this when the docs are coplete)

## Building and installing

### Dependencies
The project uses [CMake](https://cmake.org/) and is developed with **only cmake users**
in mind.
 
Beside that it depends on multiple libraries. These are divided into two categories:
**small** libraries which are build during cmake configure and **large** libraries which
need to be installed by the user.

The **small** libraries are:
[Google Test](https://github.com/google/googletest),
[msgpack](https://github.com/msgpack/msgpack-c/tree/cpp_master) and
[jsoncpp](https://github.com/open-source-parsers/jsoncpp). As previously mentioned,
these will be built during cmake configure and no further action is required from
the user.

The project also depends on **large** libraries, namely:
[Boost headers](https://www.boost.org/) and
[mfem](https://mfem.org/).

These must be provided by the user, as they take way too much time to compile,
meaning they must be installed at the users system in a location known to cmake.

On ubuntu based systems (and similarly on other systems) you can install boost using
```shell
sudo apt-get install libboost-all-dev
```

Mfem must be installed manually. The basic commands to install mfem are:
```shell
git clone https://github.com/mfem/mfem &&
mkdir mfem_build &&
cd mfem_build &&
cmake -DCMAKE_INSTALL_PREFIX=<install_dir> ../mfem &&
make install
```
Do not use `-DCMAKE_INSTALL_PREFIX=<install_dir>` if you want to install
to default directory, otherwise provide an `<install_dir>`.

### Build and install
Choose a build type, the options are `Debug` and `Release`.
If mfem and boost are not present at the default locations, point cmake to them using 
`-DCMAKE_PREFIX_PATH=<dep_directory>`. If you want to install to non standard location
provide `<install_dir>` otherwise omit this option.

Altogether, to download, build the project using cmake and install it, run:
```shell
git clone https://github.com/SachCZ/raytracer &&
mkdir raytracer_build &&
cd raytracer_build &&
cmake -DCMAKE_BUILD_TYPE=<build_type> -DCMAKE_PREFIX_PATH=<dep_directory>\
-DCMAKE_INSTALL_PREFIX=<install_dir> &&
make install
```

##Usage in your project
To use raytracer library in your cmake project simply call use a CMakeLists.txt
similar to this:
```cmake
cmake_minimum_required(VERSION 3.11)
project(executable_using_raytracer)

find_package(raytracer)

set(CMAKE_CXX_STANDARD 14)

add_executable(main main.cpp)
target_link_libraries(main raytracer::raytracer)
```

And finaly to write some code using raytracer include the header in your sources:
```c++
#include <raytracer.h>
...
```

##A minimal example

Link to [documentation](https://sachcz.github.io/raytracer).
