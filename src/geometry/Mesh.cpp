#include <utility>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <memory>

#include "Mesh.h"
#include "raytracer/utility/JsonFormatter.h"

raytracer::impl::MeshSerializer raytracer::geometry::Mesh::serializer;

raytracer::geometry::Mesh::Mesh(const std::string &filename)
        : Mesh(serializer.parseVTK(filename)) {}

raytracer::geometry::Mesh::Mesh(
        const std::vector<raytracer::geometry::Point>& referencePoints,
        std::vector<std::vector<size_t>>  quadsIndexes): quadIndexes(std::move(quadsIndexes))
{

    for (const auto& point : referencePoints){
        this->points.emplace_back(std::make_unique<Point>(point));
    }

    this->quads = this->generateQuads();
    this->annotateQuads();
    this->generateAdjacencyList();
}

std::vector<raytracer::geometry::Quadrilateral> raytracer::geometry::Mesh::getBoundary() const {
    std::vector<Quadrilateral> result;

    std::copy_if(this->quads.begin(), this->quads.end(), std::back_inserter(result),
                 [this](Quadrilateral quad) { return this->isOnBoundary(quad); });
    return result;
}

bool raytracer::geometry::Mesh::isOnBoundary(const Quadrilateral &quad) const {
    return adjacencyList.getAdjacent(quad.id).size() < 4;
}

std::vector<raytracer::geometry::Quadrilateral>
raytracer::geometry::Mesh::getAdjacent(const raytracer::geometry::Quadrilateral &quadrilateral) const {
    auto indexes = adjacencyList.getAdjacent(quadrilateral.id);
    std::vector<Quadrilateral> result;
    result.reserve(indexes.size());
    for (int i : indexes) {
        result.emplace_back(this->quads[i]);
    }
    return result;
}

const raytracer::geometry::Quadrilateral &raytracer::geometry::Mesh::operator[](const int index) const {
    return quads[index];
}

size_t raytracer::geometry::Mesh::getFacesCount() {
    return quads.size();
}

void raytracer::geometry::Mesh::annotateQuads() {
    int i = 0;
    for (auto& quad : this->quads) {
        quad.id = i++;
    }
}

void raytracer::geometry::Mesh::generateAdjacencyList() {
    for (auto &referenceQuad : this->quads) {
        for (auto &quad : this->quads) {
            if (isAdjacent(quad, referenceQuad)) {
                adjacencyList.addEdge(referenceQuad.id, quad.id);
            }
        }
    }
}

bool raytracer::geometry::Mesh::isAdjacent(const raytracer::geometry::Quadrilateral &quadA,
                                           const raytracer::geometry::Quadrilateral &quadB) const {
    if (quadA.id == quadB.id) return false;
    int matches = 0;
    for (auto &referencePoint : quadA.points) {
        for (auto &point : quadB.points) {
            if (referencePoint == point) ++matches;
        }
    }
    return matches == 2;
}

std::vector<raytracer::geometry::Point> raytracer::geometry::Mesh::getPoints() const {
    std::vector<Point> result;
    for (const auto& point : this->points){
        result.emplace_back(*point);
    }
    return result;
}

std::vector<std::vector<size_t>> raytracer::geometry::Mesh::getQuadsAsIndexes() const {
    return this->quadIndexes;
}

void raytracer::geometry::Mesh::saveToJson(const std::string &filename) const {
    serializer.saveToJson(*this, filename);
}

const std::vector<raytracer::geometry::Quadrilateral> raytracer::geometry::Mesh::getQuads() {
    return this->quads;
}

std::vector<raytracer::geometry::Quadrilateral> raytracer::geometry::Mesh::generateQuads() {
    std::vector<Quadrilateral> result;
    result.reserve(quadIndexes.size());
    for (const auto& indexes : quadIndexes){
        result.emplace_back(quadFromIndexes(indexes));
    }
    return result;
}

raytracer::geometry::Quadrilateral raytracer::geometry::Mesh::quadFromIndexes(const std::vector<size_t> &indexes) {
    std::vector<const Point*> _points;
    _points.reserve(indexes.size());
    for (const auto& index : indexes){
        _points.emplace_back(this->points[index].get());
    }
    return Quadrilateral(_points);
}

raytracer::geometry::Mesh
raytracer::impl::MeshSerializer::parseVTK(const std::string &filename) const {
    std::vector<geometry::Quadrilateral> quads;
    std::ifstream file(filename);

    if (!file.is_open()) throw std::logic_error("Failed to open mesh file!");

    std::string line;
    std::vector<geometry::Point> points;
    std::vector<std::vector<size_t>> quadsIndexes;

    bool looping_points = false;
    bool looping_quads = false;
    while (std::getline(file, line)) {
        if (strContains(line, "POINTS")) {
            looping_points = true;
        } else if (strContains(line, "POLYGONS")) {
            looping_points = false;
            looping_quads = true;
        } else if (looping_points) {
            points.emplace_back(pointFromString(line));
        } else if (looping_quads) {
            quadsIndexes.emplace_back(quadIndexesFromString(line));
        }
    }
    return geometry::Mesh(points, quadsIndexes);
}

void raytracer::impl::MeshSerializer::saveToJson(const geometry::Mesh &mesh, const std::string &filename) const {
    using formatter = utility::JsonFormatter;

    auto points = mesh.getPoints();
    auto quadsAsIndexes = mesh.getQuadsAsIndexes();

    auto pointsJson = formatter::getSequenceRepresentation(points, [](const geometry::Point &point) {
        std::vector<double> coordinates = {point.x, point.y};
        return formatter::getSequenceRepresentation(coordinates, [](double x) { return x; });
    });

    auto quadsJson = formatter::getSequenceRepresentation(
            quadsAsIndexes,
            [](const std::vector<size_t> &indexes) {
                return formatter::getSequenceRepresentation(indexes, [](int i) { return i; });
            });

    std::map<std::string, std::string> jsonObject = {{"points",         pointsJson},
                                                     {"quadrilaterals", quadsJson}};

    std::ofstream file(filename + ".json");
    file << formatter::getObjectRepresentation(jsonObject);
}

bool raytracer::impl::MeshSerializer::strContains(const std::string &text, const std::string &toFind) const {
    return text.find(toFind) != std::string::npos;
}

raytracer::geometry::Point
raytracer::impl::MeshSerializer::pointFromString(const std::string &pointRepresentation) const {
    double x, y;
    std::string _;
    std::stringstream stream(pointRepresentation);
    stream >> x;
    stream >> y;
    return {x, y};
}

std::vector<size_t>
raytracer::impl::MeshSerializer::quadIndexesFromString(const std::string &quadIndexesRepresentation) const {
    size_t i, j, k, l;
    std::string _;
    std::stringstream stream(quadIndexesRepresentation);
    stream >> _;
    stream >> i;
    stream >> j;
    stream >> k;
    stream >> l;
    return {i, j, k, l};
}