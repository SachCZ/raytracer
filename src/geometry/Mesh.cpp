#include <algorithm>
#include <fstream>
#include <sstream>

#include "Mesh.h"
#include "raytracer/utility/JsonFormatter.h"

raytracer::impl::MeshSerializer raytracer::geometry::Mesh::serializer;

raytracer::geometry::Mesh::Mesh(const std::string &filename)
        : Mesh(serializer.parseVTK(filename)) {}

raytracer::geometry::Mesh::Mesh(std::vector<raytracer::geometry::Quadrilateral> quadrilaterals) :
        quads(std::move(quadrilaterals)) {
    this->annotateQuads();
    this->generateAdjacencyList();
}

std::vector<raytracer::geometry::Quadrilateral> raytracer::geometry::Mesh::getBoundary() const {
    std::vector<Quadrilateral> result;

    std::copy_if(this->quads.begin(), this->quads.end(), std::back_inserter(result),
                 [this](Quadrilateral quad) { return this->isOnBoundary(quad); });
    return result;
}

bool raytracer::geometry::Mesh::isOnBoundary(const Quadrilateral &triangle) const {
    return adjacencyList.getAdjacent(triangle.id).size() < 4;
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
    for (auto &quad : this->quads) {
        quad.id = i++;
    }
}

void raytracer::geometry::Mesh::generateAdjacencyList() {
    for (auto &referenceQuad : quads) {
        for (auto &quad : quads) {
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

std::vector<raytracer::geometry::Point> raytracer::geometry::Mesh::getAllPoints() const {
    std::vector<Point> points;
    for (auto &quad : this->quads) {
        for (auto &point : quad.getPoints()) {
            if (std::find(points.begin(), points.end(), point) == points.end()) {
                points.emplace_back(point);
            }
        }
    }
    return points;
}

std::vector<std::set<size_t>> raytracer::geometry::Mesh::getQuadsAsIndexes() const {
    std::vector<Point> points = this->getAllPoints();
    std::vector<std::set<size_t>> quadsAsIndexes;
    for (auto &quad : this->quads) { //TODO separate this
        std::set<size_t> indexes;
        for (auto &point : quad.getPoints()) {
            auto it = std::find(points.begin(), points.end(), point);
            indexes.emplace(std::distance(points.begin(), it));
        }
        quadsAsIndexes.emplace_back(indexes);
    }
    return quadsAsIndexes;
}

void raytracer::geometry::Mesh::saveToJson(const std::string &filename) const {
    serializer.saveToJson(*this, filename);
}

const std::vector<raytracer::geometry::Quadrilateral> raytracer::geometry::Mesh::getQuads() {
    return this->quads;
}

std::vector<raytracer::geometry::Quadrilateral>
raytracer::impl::MeshSerializer::parseVTK(const std::string &filename) const {
    std::vector<geometry::Quadrilateral> quads;
    std::ifstream file(filename);

    if (!file.is_open()) throw std::logic_error("Failed to open mesh file!");

    std::string line;
    std::vector<geometry::Point> points;

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
            auto indexes = quadIndexesFromString(line);
            quads.emplace_back(quadFromIndexes(indexes, points));
        }
    }
    return quads;
}

void raytracer::impl::MeshSerializer::saveToJson(const geometry::Mesh &mesh, const std::string &filename) const {
    using formatter = utility::JsonFormatter;

    auto points = mesh.getAllPoints();
    auto quadsAsIndexes = mesh.getQuadsAsIndexes();

    auto pointsJson = formatter::getSequenceRepresentation(points, [](const geometry::Point &point) {
        std::vector<double> coordinates = {point.x, point.y};
        return formatter::getSequenceRepresentation(coordinates, [](double x) { return x; });
    });

    auto quadsJson = formatter::getSequenceRepresentation(
            quadsAsIndexes,
            [](const std::set<size_t> &indexes) {
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

raytracer::geometry::Quadrilateral raytracer::impl::MeshSerializer::quadFromIndexes(
        const std::vector<size_t> &indexes,
        const std::vector<raytracer::geometry::Point> &points) const {

    std::vector<geometry::Point> quadPoints(4);
    std::transform(indexes.begin(), indexes.end(), quadPoints.begin(), [&points](size_t index) {
        return points[index];
    });
    return geometry::Quadrilateral(quadPoints);
}
