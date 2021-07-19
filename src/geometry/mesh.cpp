#include "mesh.h"
#include <memory>
#include <set>
#include <utility.h>
#include <stdexcept>

namespace raytracer {
    std::vector<Face *> MfemMesh::getBoundary() const {
        return this->boundaryFaces;
    }

    MfemMesh::MfemMesh(mfem::Mesh *mesh) : mesh(mesh) { this->init(); }

    MfemMesh::MfemMesh(const std::string &filename, bool generateEdges, bool refine) :
            mfemMesh(make_unique<mfem::Mesh>(filename.c_str(), generateEdges, refine)),
            mesh(mfemMesh.get()) {
        this->init();
    }

    std::unique_ptr<Element> MfemMesh::createElementFromId(int id) const {
        if (id == -1) return nullptr;

        mfem::Array<int> facesIds;
        mfem::Array<int> _;
        mfem::Array<int> verticesIds;

        if (this->mesh->Dimension() == 2) {
            this->mesh->GetElementEdges(id, facesIds, _);
        } else if (this->mesh->Dimension() == 3) {
            this->mesh->GetElementFaces(id, facesIds, _);
        }
        this->mesh->GetElementVertices(id, verticesIds);

        return make_unique<Element>(id, this->getFacesFromIds(facesIds), this->getPointsFromIds(verticesIds));
    }

    std::unique_ptr<Point> MfemMesh::createPointFromId(int id) const { //TODO refactor this, dimension specific
        double coords[2];
        mesh->GetNode(id, coords);
        return make_unique<Point>(coords[0], coords[1], id);
    }

    void MfemMesh::updateMesh() {
        for (auto& point : this->points){
            double coords[2];
            mesh->GetNode(point->id, coords);
            point->x = coords[0];
            point->y = coords[1];
        }
    }

    std::vector<Point *> MfemMesh::getPointsFromIds(const mfem::Array<int> &ids) const {
        std::vector<Point *> result;
        for (auto id : ids) {
            result.emplace_back(this->points[id].get());
        }
        return result;
    }

    std::unique_ptr<Face> MfemMesh::createFaceFromId(int id) const {
        mfem::Array<int> faceVerticesIds;
        this->mesh->GetFaceVertices(id, faceVerticesIds);

        std::unique_ptr<Face> face(new Face(id, this->getPointsFromIds(faceVerticesIds)));
        return face;
    }

    std::vector<Face *> MfemMesh::genBoundaryFaces() {
        std::vector<Face *> result;
        for (int i = 0; i < this->mesh->GetNumFaces(); ++i) {
            int id1, id2;
            this->mesh->GetFaceElements(i, &id1, &id2);
            if (id1 == -1 || id2 == -1) {
                result.emplace_back(this->faces[i].get());
            }
        }
        return result;
    }

    Element *MfemMesh::getElementFromId(int id) const {
        if (id < 0) return nullptr;
        else return this->elements[id].get();
    }

    std::vector<std::unique_ptr<Point>> MfemMesh::genPoints() {
        std::vector<std::unique_ptr<Point>> result;
        result.reserve(mesh->GetNV());
        for (int id = 0; id < mesh->GetNV(); ++id) {
            result.emplace_back(this->createPointFromId(id));
        }
        return result;
    }

    std::vector<std::unique_ptr<Face>> MfemMesh::genFaces() {
        std::vector<std::unique_ptr<Face>> result;
        int facesCount = this->mesh->Dimension() == 2 ? this->mesh->GetNEdges() : this->mesh->GetNFaces();

        result.reserve(facesCount);
        for (int id = 0; id < facesCount; ++id) {
            result.emplace_back(this->createFaceFromId(id));
        }
        return result;
    }

    std::vector<std::unique_ptr<Element>> MfemMesh::genElements() {
        std::vector<std::unique_ptr<Element>> result;

        result.reserve(mesh->GetNE());
        for (int id = 0; id < mesh->GetNE(); ++id) {
            result.emplace_back(this->createElementFromId(id));
        }
        return result;
    }

    std::vector<Face *> MfemMesh::getFacesFromIds(const mfem::Array<int> &ids) const {
        std::vector<Face *> result;
        size_t size = std::distance(ids.begin(), ids.end());
        result.reserve(size);
        for (auto id : ids) {
            result.emplace_back(this->faces[id].get());
        }
        return result;
    }

    Element *MfemMesh::getFaceDirAdjElement(const Face *face, const Vector &direction) const {
        auto adjacent = getFaceAdjElements(face);
        auto normal = face->getNormal();

        if (normal * direction < 0) {
            return adjacent.first;
        } else {
            return adjacent.second;
        }
    }

    std::vector<Element *> MfemMesh::getElementAdjacentElements(const Element &element) const {
        mfem::Array<int> elementIds(
                this->elementToElementTable.GetRow(element.getId()),
                this->elementToElementTable.RowSize(element.getId())
        );
        std::vector<Element *> result;
        result.reserve(elementIds.Size());
        for (auto id : elementIds) {
            result.emplace_back(this->getElementFromId(id));
        }
        return result;
    }

    void MfemMesh::setBoundaryAndInner() {
        std::set<Point *> boundaryPointsSet;
        for (const auto &face : this->boundaryFaces) {
            for (const auto &point : face->getPoints()) {
                boundaryPointsSet.insert(point);
            }
        }
        for (const auto &point : this->points) {
            if (boundaryPointsSet.find(point.get()) == boundaryPointsSet.end()) {
                innerPoints.emplace_back(point.get());
            }
        }
        boundaryPoints.assign(boundaryPointsSet.begin(), boundaryPointsSet.end());
    }

    std::vector<Point *> MfemMesh::getInnerPoints() const {
        return this->innerPoints;
    }

    std::vector<Point *> MfemMesh::getBoundaryPoints() const {
        return this->boundaryPoints;
    }

    std::vector<Element *> MfemMesh::precalcPointAdjacentElements(const Point *point) const {
        int pointId = -1;
        for (uint i = 0; i < this->points.size(); i++) {
            if (point == this->points[i].get()) {
                pointId = i;
                break;
            }
        }

        mfem::Array<int> elementIds(
                this->vertexToElementTable->GetRow(pointId),
                this->vertexToElementTable->RowSize(pointId)
        );
        std::vector<Element *> result;
        result.reserve(elementIds.Size());
        for (auto id : elementIds) {
            result.emplace_back(this->getElementFromId(id));
        }
        return result;
    }

    std::vector<Point *> MfemMesh::getPoints() const {
        std::vector<Point *> result;
        std::transform(this->points.begin(), this->points.end(), std::back_inserter(result), [](const std::unique_ptr<Point>& point) {
            return point.get();
        });
        return result;
    }

    mfem::Mesh *MfemMesh::getMfemMesh() const {
        return this->mesh;
    }

    std::vector<Element *> MfemMesh::getElements() const {
        std::vector<Element *> result;
        std::transform(
                this->elements.begin(), this->elements.end(), std::back_inserter(result),
                [](const std::unique_ptr<Element>& element) {
                    return element.get();
                }
        );
        return result;
    }

    void MfemMesh::init() {
        this->elementToElementTable = mesh->ElementToElementTable();
        this->vertexToElementTable = std::unique_ptr<mfem::Table>(mesh->GetVertexToElementTable());
        this->points = this->genPoints();
        this->faces = this->genFaces();
        this->elements = this->genElements();
        this->boundaryFaces = this->genBoundaryFaces();
        this->setBoundaryAndInner();
        this->pointsAdjacentElements = this->genPointsAdjacentElements();
    }

    std::map<const Point *, std::vector<Element *>> MfemMesh::genPointsAdjacentElements() const {
        std::map<const Point *, std::vector<Element *>> result;
        for (const Point *point: this->getPoints()) {
            result[point] = precalcPointAdjacentElements(point);
        }
        return result;
    }

    std::vector<Element *> MfemMesh::getPointAdjacentElements(const Point *point) const {
        return pointsAdjacentElements.at(point);
    }

    std::pair<Element *, Element *> MfemMesh::getFaceAdjElements(const Face *face) const {
        int elementA, elementB;
        this->mesh->GetFaceElements(face->getId(), &elementA, &elementB);

        return {getElementFromId(elementA), getElementFromId(elementB)};
    }

    std::pair<Face *, Face *> MfemMesh::getSharedFaces(const Point *point, const Element &element) {
        std::pair<Face *, Face *> result{};
        for (auto face : element.getFaces()) {
            const auto &_points = face->getPoints();
            if (point == _points[0] || point == _points[1]) {
                if (result.first == nullptr) {
                    result.first = face;
                } else if (result.second == nullptr) {
                    result.second = face;
                    return result;
                }
            }
        }
        throw std::logic_error("Unreachable code!");
    }

    std::vector<Face *> MfemMesh::getPointAdjOrderedFaces(const Point *point) const {
        auto adjElements = this->getPointAdjacentElements(point);
        std::vector<Face *> orderedFaces;
        orderedFaces.reserve(adjElements.size());

        std::set<Face *> visitedFaces;
        auto currentElement = adjElements[0];
        while (true) {
            auto adjFaces = getSharedFaces(point, *currentElement);
            Face *chosenFace{};
            if (visitedFaces.find(adjFaces.first) == visitedFaces.end()) {
                chosenFace = adjFaces.first;
            } else if (visitedFaces.find(adjFaces.second) == visitedFaces.end()) {
                chosenFace = adjFaces.second;
            } else {
                break;
            }
            visitedFaces.emplace(chosenFace);
            orderedFaces.emplace_back(chosenFace);

            auto adj = this->getFaceAdjElements(chosenFace);
            currentElement = adj.first != currentElement ? adj.first : adj.second;
        }
        //determine if clockwise
        auto a = orderedFaces[0]->getPoints()[0]->id == point->id ? orderedFaces[0]->getPoints()[1]
                                                                  : orderedFaces[0]->getPoints()[0];
        auto b = orderedFaces[1]->getPoints()[0]->id == point->id ? orderedFaces[1]->getPoints()[1]
                                                                  : orderedFaces[1]->getPoints()[0];

        auto vecA = Vector(*a - *point);
        auto vecB = Vector(*b - *point);

        if (vecA.getNormal().crossZ(vecB.getNormal()) < 0) {
            std::reverse(std::begin(orderedFaces), std::end(orderedFaces));
        }

        return orderedFaces;
    }

    std::vector<Element *> MfemMesh::getPointAdjOrderedElements(const Point *point) const {
        using namespace std;
        std::vector<Element *> result;
        auto adjFaces = getPointAdjOrderedFaces(point);
        std::set<Element *> visitedElements;
        for (auto it = begin(adjFaces); it != end(adjFaces); it++) {
            auto nextIt = next(it);
            if (nextIt == end(adjFaces)) {
                nextIt = begin(adjFaces);
            }
            auto adjEle1 = this->getFaceAdjElements(*it);
            auto adjEle2 = this->getFaceAdjElements(*nextIt);
            if (adjEle1.first == adjEle2.first || adjEle1.first == adjEle2.second) {
                result.emplace_back(adjEle1.first);
            } else {
                result.emplace_back(adjEle1.second);
            }
        }
        return result;
    }

    MfemMesh::MfemMesh(
            SegmentedLine sideA,
            SegmentedLine sideB,
            mfem::Element::Type elementType
    ) : mfemMesh(make_unique<mfem::Mesh>(
            sideA.segmentCount,
            sideB.segmentCount,
            elementType,
            true,
            sideA.end - sideA.start,
            sideB.end - sideB.start,
            true)),
        mesh(mfemMesh.get()) {

        auto nodesCount = mfemMesh->GetNV();
        Displacements displacements(nodesCount, {sideA.start, sideB.start});
        this->moveNodes(displacements);

        this->init();
    }

    void MfemMesh::moveNodes(const MfemMesh::Displacements &displacements) {
        auto verticesCount = mfemMesh->GetNV();
        mfem::Vector _displacements(verticesCount * 2);
        for (int i = 0; i < verticesCount; i++) {
            _displacements[i] = displacements[i].x;
            _displacements[i + verticesCount] = displacements[i].y;
        }
        mfemMesh->MoveVertices(_displacements);
        this->updateMesh();
    }

    std::ostream &operator<<(std::ostream &os, const MfemMesh &mesh) {
        mesh.getMfemMesh()->Print(os);
        return os;
    }

    std::ostream &writeDualMesh(std::ostream &os, const Mesh &mesh) {
        using namespace std;
        stringstream vertices;
        const auto &elements = mesh.getElements();

        vertices << "vertices\n" << elements.size() << "\n2\n";
        for (const Element *element : elements) {
            vertices << getElementCentroid(*element) << "\n";
        }
        const auto &points = mesh.getInnerPoints();
        stringstream elementsString;
        elementsString << "elements\n" << points.size() << "\n";
        for (const Point *point : mesh.getInnerPoints()) {
            auto adjacentElements = mesh.getPointAdjacentElements(point);
            std::string elementPrefix;
            if (adjacentElements.size() == 3) {
                elementPrefix = "1 2";
            } else if (adjacentElements.size() == 4) {
                elementPrefix = "1 3";
            } else {
                throw logic_error("Unsupported element type!");
            }
            elementsString << elementPrefix;
            std::sort(adjacentElements.begin(), adjacentElements.end(), [](const Element *a, const Element* b){
                return a->getId() < b->getId();
            });
            for (const Element *element : adjacentElements) {
                elementsString << " " << element->getId();
            }
            elementsString << "\n";
        }

        os << "MFEM mesh v1.0\n\ndimension\n2\n\n";
        os << elementsString.str() << "\n\n";
        os << "boundary\n0\n\n";
        os << vertices.str();
        return os;
    }
}
