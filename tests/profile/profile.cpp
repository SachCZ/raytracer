#include <raytracer/geometry/Mesh.h>
#include <raytracer/geometry/GeometryFunctions.h>
#include <raytracer/geometry/Ray.h>
#include <chrono>
#include <iostream>

int main(int argc, char *argv[]) {
    using namespace raytracer::geometry;

    DiscreteLine sideA{10.0, 100};
    auto mfemMesh = constructRectangleMesh(sideA, sideA);
    Mesh mesh{mfemMesh.get()};
    Line orientation{Point(-1, 4.5), Vector(1, 0)};
    Ray ray{orientation};
    size_t steps = 10000;

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < steps; ++i){
        auto intersections = ray.findIntersections(
                mesh,
                [](const Intersection& previousIntersection) -> std::unique_ptr<Intersection> {
                    const auto element = previousIntersection.nextElement;
                    return findClosestIntersection(previousIntersection.orientation, element->getFaces());
                },
                [](const Intersection& previousIntersection) {
                    return false;
                }
        );
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto nanosecondDuration = std::chrono::duration_cast<std::chrono::nanoseconds>( end - start ).count();
    auto duration = nanosecondDuration * 1e-9;

    std::cout << "Execution took: " << duration << "s." << std::endl;
    std::cout << "That is: " << duration / steps << "s per step." << std::endl;
}