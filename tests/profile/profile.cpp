#include <raytracer/geometry/Mesh.h>

using Mesh = raytracer::geometry::Mesh;

int main(int argc, char* argv[])
{
    Mesh mesh("./mesh.stl");
    auto triangles = mesh.getQuads();

    for (size_t i = 0; i < 1000; ++i){
        Mesh newMesh(triangles);
    }

}