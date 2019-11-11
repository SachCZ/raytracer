#include "Mesh.h"

raytracer::geometry::Mesh::Mesh(const std::string &filename, double tolerance)
        : Mesh(parseSTL(filename), tolerance)
{}

raytracer::geometry::Mesh::Mesh(std::vector<raytracer::geometry::Triangle> triangles, double tolerance) :
        triangles(std::move(triangles)), tolerance(tolerance)
{
    this->annotateTriangles();
    this->annotateEdges();
    this->generateAdjacencyList();
    this->generateBoundaryEdgesList();
}

std::vector<raytracer::geometry::Triangle> raytracer::geometry::Mesh::getBoundary() const {
    std::vector<Triangle> result;

    std::copy_if(this->triangles.begin(), this->triangles.end(), std::back_inserter(result),
                 [this](Triangle triangle){return this->isOnBoundary(triangle);});
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

bool raytracer::geometry::Mesh::isBoundary(const Edge &edge) const {
    return this->boundaryEges.count(edge.id) > 0;
}

const raytracer::geometry::Triangle &raytracer::geometry::Mesh::operator[](const int index) const {
    return triangles[index];
}

size_t raytracer::geometry::Mesh::getFacesCount() {
    return triangles.size();
}

void raytracer::geometry::Mesh::annotateTriangles() {
    int i = 0;
    for (auto& triangle : this->triangles){
        triangle.id = i++;
    }
}

void raytracer::geometry::Mesh::generateAdjacencyList() {
    for (auto& referenceTriangle : triangles){
        for (auto& triangle : triangles){
            if (isAdjacent(triangle, referenceTriangle)){
                adjacencyList.addEdge(referenceTriangle.id, triangle.id);
            }
        }
    }
}

bool raytracer::geometry::Mesh::isAdjacent(const raytracer::geometry::Triangle &triangleA,
                                           const raytracer::geometry::Triangle &triangleB) const {
    if (triangleA.id == triangleB.id) return false;
    int matches = 0;
    for (auto& referencePoint : triangleA.points){
        for (auto& point : triangleB.points){
            if (referencePoint.isEqual(point, this->tolerance)) ++matches;
        }
    }
    return matches == 2;
}

raytracer::geometry::Point raytracer::geometry::Mesh::pointFromString(const std::string &pointRepresentation) const {
    double x, y;
    std::string _;
    std::stringstream stream(pointRepresentation);
    stream >> _;
    stream >> x;
    stream >> y;
    return {x, y};
}

std::vector<raytracer::geometry::Triangle> raytracer::geometry::Mesh::parseSTL(const std::string &filename) const {
    std::vector<Triangle> _triangles;
    std::ifstream file(filename);
    std::string line;
    std::vector<Point> points;
    bool inside_loop = false;
    points.reserve(3);
    while(std::getline(file, line)){
        if (str_contains(line, "loop")){
            inside_loop = true;
        }
        if (inside_loop && str_contains(line, "vertex")) {
            points.emplace_back(pointFromString(line));
        }
        if (str_contains(line, "endloop")) {
            _triangles.emplace_back(Triangle(points));
            points.clear();
            inside_loop = false;
        }
    }
    return _triangles;
}

bool raytracer::geometry::Mesh::str_contains(const std::string &text, const std::string &toFind) const {
    return text.find(toFind) != std::string::npos;
}


std::vector<raytracer::geometry::Point> raytracer::geometry::Mesh::getAllPoints() const {
    std::vector<Point> points;
    for (auto& triangle : this->triangles){
        for (auto& trianglePoint : triangle.getPoints()){
            if (!this->contains(points, trianglePoint)) points.emplace_back(trianglePoint);
        }
    }
    return points;
}

std::vector<std::unordered_set<size_t>> raytracer::geometry::Mesh::getTrianglesAsIndexes() const {
    std::vector<Point> points = this->getAllPoints();
    std::vector<std::unordered_set<size_t>> trianglesAdIndexes;
    for (auto& triangle : this->triangles){
        std::unordered_set<size_t> indexes;
        for (auto& point : triangle.getPoints()){
            for (size_t i = 0; i < points.size(); ++i){
                if (point.isEqual(points[i], this->tolerance)){
                    indexes.insert(i);
                    break;
                }
            }
        }
        trianglesAdIndexes.emplace_back(indexes);
    }
    return trianglesAdIndexes;
}

void raytracer::geometry::Mesh::saveToJson(const std::string &filename) const {
    using formatter = utility::JsonFormatter;

    auto points = getAllPoints();
    auto trianglesAsIndexes = getTrianglesAsIndexes();

    auto pointsJson = formatter::getSequenceRepresentation(points, [](const Point& point){
        std::vector<double> coordinates = {point.x, point.y};
        return formatter::getSequenceRepresentation(coordinates, [](double x){return x;});
    });

    auto trianglesJson = formatter::getSequenceRepresentation(trianglesAsIndexes,
                                                              [](const std::unordered_set<size_t>& indexes){
                                                                  return formatter::getSequenceRepresentation(indexes, [](int i){return i;});
                                                              });

    std::map<std::string, std::string> jsonObject = {{"points", pointsJson}, {"triangles", trianglesJson}};

    std::ofstream file(filename + ".json");
    file << formatter::getObjectRepresentation(jsonObject);
}

void raytracer::geometry::Mesh::annotateEdges() {
    auto edges = this->getAllEdges();
    for (auto& triangle : this->triangles){
        for (auto& triangleEdge : triangle.edges){
            for (size_t i = 0; i < edges.size(); ++i){
                const auto& referenceEdge = edges[i];
                if (referenceEdge.isEqual(triangleEdge, this->tolerance)){
                    triangleEdge.id = i;
                }
            }
        }
    }
}

std::vector<raytracer::geometry::Edge> raytracer::geometry::Mesh::getAllEdges() const {
    std::vector<Edge> edges;
    for (auto& triangle : triangles){
        for (auto& edge : triangle.edges){
            if (!this->contains(edges, edge)) edges.emplace_back(edge);
        }
    }
    return edges;
}

template <typename Element>
bool raytracer::geometry::Mesh::contains(const std::vector<Element>& array, const Element &element) const {
    bool contains = false;
    for (const auto& referenceElement : array){
        if (referenceElement.isEqual(element, this->tolerance)){
            return true;
        }
    }
    return contains;
}

void raytracer::geometry::Mesh::generateBoundaryEdgesList() {
    auto allEdges = this->getAllEdges();
    std::unordered_set<size_t> indexes;
    indexes.reserve(allEdges.size());
    std::for_each(allEdges.begin(), allEdges.end(), [&](const Edge& edge){indexes.insert(edge.id);});
    std::unordered_set<size_t> visitedIndexes;

    for (auto& triangle : this->triangles){
        for (auto& edge : triangle.edges){
            if (visitedIndexes.count(edge.id) > 0){
                indexes.erase(edge.id);
            } else {
                visitedIndexes.emplace(edge.id);
            }
        }
    }
    this->boundaryEges = indexes;
}


