#include "Mesh.h"
#include <set>
#include <vector>

raytracer::geometry::Mesh::Mesh(const std::string &filename)
        : Mesh(parseSTL(filename))
{}

raytracer::geometry::Mesh::Mesh(std::vector<raytracer::geometry::Triangle> triangles) :
        triangles(std::move(triangles))
{
    this->annotateTriangles();

    this->edges = this->getAllEdges();
    this->annotateEdges();
    this->boundaryEdges = this->getBoundaryEdgesIndexes();

    this->generateAdjacencyList();
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
    return std::find(this->boundaryEdges.begin(), this->boundaryEdges.end(), edge.id) != this->boundaryEdges.end();
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
            if (referencePoint == point) ++matches;
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

    if (!file.is_open()) throw std::logic_error("Failed to open mesh file!");

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
        for (auto& point : triangle.getPoints()){
            if (std::find(points.begin(), points.end(), point) == points.end()){
                points.emplace_back(point);
            }
        }
    }
    return points;
}

std::vector<std::set<size_t>> raytracer::geometry::Mesh::getTrianglesAsIndexes() const {
    std::vector<Point> points = this->getAllPoints();
    std::vector<std::set<size_t>> trianglesAsIndexes;
    for (auto& triangle : this->triangles){
        std::set<size_t> indexes;
        for (auto& point : triangle.getPoints()){
            auto it = std::find(points.begin(), points.end(), point);
            indexes.emplace(std::distance(points.begin(), it));
        }
        trianglesAsIndexes.emplace_back(indexes);
    }
    return trianglesAsIndexes;
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
                                                              [](const std::set<size_t>& indexes){
                                                                  return formatter::getSequenceRepresentation(indexes, [](int i){return i;});
                                                              });

    std::map<std::string, std::string> jsonObject = {{"points", pointsJson}, {"triangles", trianglesJson}};

    std::ofstream file(filename + ".json");
    file << formatter::getObjectRepresentation(jsonObject);
}

void raytracer::geometry::Mesh::annotateEdges() {
    std::vector<Edge> uniqueEdges;
    std::for_each(this->edges.begin(), this->edges.end(), [&uniqueEdges](const Edge* edge) {
        if (std::find(uniqueEdges.begin(), uniqueEdges.end(), *edge) == uniqueEdges.end()){
            uniqueEdges.emplace_back(*edge);
        }
    });
    std::for_each(uniqueEdges.begin(), uniqueEdges.end(), [n=0](Edge& edge) mutable {edge.id=n++;});
    std::for_each(this->edges.begin(), this->edges.end(), [uniqueEdges](Edge* edge) mutable {
        auto it = std::find(uniqueEdges.begin(), uniqueEdges.end(), *edge);
        edge->id = it->id;
    });
}

template <typename Function>
void raytracer::geometry::Mesh::iterateTriangleEdges(Function function){
    for (auto& triangle : this->triangles){
        for (auto& edge : triangle.edges){
            function(edge);
        }
    }
}

std::vector<raytracer::geometry::Edge*> raytracer::geometry::Mesh::getAllEdges() {
    std::vector<Edge*> result;
    this->iterateTriangleEdges([&result](Edge& edge){
        result.emplace_back(&edge);
    });
    return result;
}

std::vector<size_t> raytracer::geometry::Mesh::getBoundaryEdgesIndexes() {
    std::vector<size_t> indexes(this->edges.size());
    std::transform(this->edges.begin(), this->edges.end(), indexes.begin(), [](const Edge* edge){return edge->id;});
    return this->findUniques(indexes);
}

template <typename Type>
std::vector<Type> raytracer::geometry::Mesh::findUniques(std::vector<Type> array) {
    std::vector<Type> result;
    std::sort(array.begin(), array.end());
    for (size_t i = 1; i < array.size() - 1; ++i){
        if (array[i-1] != array[i] && array[i] != array[i+1]){
            result.emplace_back(array[i]);
        }
    }
    return result;
}


