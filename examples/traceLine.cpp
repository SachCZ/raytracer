#include "geometry/Ray.h"
#include "geometry/Mesh.h"

using Mesh = raytracer::geometry::Mesh;
using Ray = raytracer::geometry::Ray;
using Point = raytracer::geometry::Point;
using Vector = raytracer::geometry::Vector;
using Intersection = raytracer::geometry::Intersection;

int main(int argc, char* argv[])
{
    Mesh mesh("./mesh.stl");
    Ray ray(Point(-2, 0.1), Vector(1, 0));
    ray.traceThrough(mesh, [](const Intersection& intersection){
        return Vector(1, 0.5 * intersection.point.x);
    });
    ray.saveToJson("ray");
    mesh.saveToJson("mesh");
}