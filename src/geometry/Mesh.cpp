#include <algorithm>
#include <fstream>
#include <sstream>

#include "Mesh.h"
#include "raytracer/utility/JsonFormatter.h"

raytracer::impl::MeshSerializer raytracer::geometry::Mesh::serializer;

raytracer::geometry::Mesh::Mesh(const std::string &filename)
        : Mesh(serializer.parseSTL(filename)) {}

raytracer::geometry::Mesh::Mesh(std::vector<raytracer::geometry::Triangle> triangles) :
        triangles(std::move(triangles)) {
    this->annotateTriangles();
    this->generateAdjacencyList();
}

std::vector<raytracer::geometry::Triangle> raytracer::geometry::Mesh::getBoundary() const {
    std::vector<Triangle> result;

    std::copy_if(this->triangles.begin(), this->triangles.end(), std::back_inserter(result),
                 [this](Triangle triangle) { return this->isOnBoundary(triangle); });
    return result;
}

bool raytracer::geometry::Mesh::isOnBoundary(const Triangle &triangle) const {
    return adjacencyList.getAdjacent(triangle.id).size() < 3;
}

std::vector<raytracer::geometry::Triangle>
raytracer::geometry::Mesh::getAdjacent(const raytracer::geometry::Triangle &triangle) const {
    auto indexes = adjacencyList.getAdjacent(triangle.id);
    std::vector<Triangle> result;
    result.reserve(indexes.size());
    for (int i : indexes) {
        result.emplace_back(this->triangles[i]);
    }
    return result;
}

const raytracer::geometry::Triangle &raytracer::geometry::Mesh::operator[](const int index) const {
    return triangles[index];
}

size_t raytracer::geometry::Mesh::getFacesCount() {
    return triangles.size();
}

void raytracer::geometry::Mesh::annotateTriangles() {
    int i = 0;
    for (auto &triangle : this->triangles) {
        triangle.id = i++;
    }
}

void raytracer::geometry::Mesh::generateAdjacencyList() {
    for (auto &referenceTriangle : triangles) {
        for (auto &triangle : triangles) {
            if (isAdjacent(triangle, referenceTriangle)) {
                adjacencyList.addEdge(referenceTriangle.id, triangle.id);
            }
        }
    }
}

bool raytracer::geometry::Mesh::isAdjacent(const raytracer::geometry::Triangle &triangleA,
                                           const raytracer::geometry::Triangle &triangleB) const {
    if (triangleA.id == triangleB.id) return false;
    int matches = 0;
    for (auto &referencePoint : triangleA.points) {
        for (auto &point : triangleB.points) {
            if (referencePoint == point) ++matches;
        }
    }
    return matches == 2;
}

std::vector<raytracer::geometry::Point> raytracer::geometry::Mesh::getAllPoints() const {
    std::vector<Point> points;
    for (auto &triangle : this->triangles) {
        for (auto &point : triangle.getPoints()) {
            if (std::find(points.begin(), points.end(), point) == points.end()) {
                points.emplace_back(point);
            }
        }
    }
    return points;
}

std::vector<std::set<size_t>> raytracer::geometry::Mesh::getTrianglesAsIndexes() const {
    std::vector<Point> points = this->getAllPoints();
    std::vector<std::set<size_t>> trianglesAsIndexes;
    for (auto &triangle : this->triangles) {
        std::set<size_t> indexes;
        for (auto &point : triangle.getPoints()) {
            auto it = std::find(points.begin(), points.end(), point);
            indexes.emplace(std::distance(points.begin(), it));
        }
        trianglesAsIndexes.emplace_back(indexes);
    }
    return trianglesAsIndexes;
}

void raytracer::geometry::Mesh::saveToJson(const std::string &filename) const {
    serializer.saveToJson(*this, filename);
}

std::vector<raytracer::geometry::Triangle>
raytracer::impl::MeshSerializer::parseSTL(const std::string &filename) const {
    std::vector<geometry::Triangle> _triangles;
    std::ifstream file(filename);

    if (!file.is_open()) throw std::logic_error("Failed to open mesh file!");

    std::string line;
    std::vector<geometry::Point> points;
    bool inside_loop = false;
    points.reserve(3);
    while (std::getline(file, line)) {
        if (str_contains(line, "loop")) {
            inside_loop = true;
        }
        if (inside_loop && str_contains(line, "vertex")) {
            points.emplace_back(pointFromString(line));
        }
        if (str_contains(line, "endloop")) {
            _triangles.emplace_back(geometry::Triangle(points));
            points.clear();
            inside_loop = false;
        }
    }
    return _triangles;
}

void raytracer::impl::MeshSerializer::saveToJson(const geometry::Mesh &mesh, const std::string &filename) const {
    using formatter = utility::JsonFormatter;

    auto points = mesh.getAllPoints();
    auto trianglesAsIndexes = mesh.getTrianglesAsIndexes();

    auto pointsJson = formatter::getSequenceRepresentation(points, [](const geometry::Point &point) {
        std::vector<double> coordinates = {point.x, point.y};
        return formatter::getSequenceRepresentation(coordinates, [](double x) { return x; });
    });

    auto trianglesJson = formatter::getSequenceRepresentation(
            trianglesAsIndexes,
            [](const std::set<size_t> &indexes) {
                return formatter::getSequenceRepresentation(indexes, [](int i) { return i; });
            });

    std::map<std::string, std::string> jsonObject = {{"points",    pointsJson},
                                                     {"triangles", trianglesJson}};

    std::ofstream file(filename + ".json");
    file << formatter::getObjectRepresentation(jsonObject);
}

bool raytracer::impl::MeshSerializer::str_contains(const std::string &text, const std::string &toFind) const {
    return text.find(toFind) != std::string::npos;
}

raytracer::geometry::Point
raytracer::impl::MeshSerializer::pointFromString(const std::string &pointRepresentation) const {
    double x, y;
    std::string _;
    std::stringstream stream(pointRepresentation);
    stream >> _;
    stream >> x;
    stream >> y;
    return {x, y};
}
