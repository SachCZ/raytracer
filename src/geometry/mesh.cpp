#include "mesh.h"
#include <set>

namespace raytracer {
    std::vector<Face *> MfemMesh::getBoundary() const {
        return this->boundaryFaces;
    }

    MfemMesh::MfemMesh(mfem::Mesh *mesh) : mesh(mesh) { this->init(); }

    MfemMesh::MfemMesh(
            DiscreteLine sideA,
            DiscreteLine sideB,
            mfem::Element::Type elementType
    ) : mfemMesh(std::make_unique<mfem::Mesh>(
            sideA.segmentCount,
            sideB.segmentCount,
            elementType,
            true,
            sideA.length,
            sideB.length,
            true)),
        mesh(mfemMesh.get()) { this->init(); }

    std::unique_ptr<Element> MfemMesh::createElementFromId(int id) const {
        if (id == -1) return nullptr;

        mfem::Array<int> facesIds;
        mfem::Array<int> _;

        if (this->mesh->Dimension() == 2) {
            this->mesh->GetElementEdges(id, facesIds, _);
        } else if (this->mesh->Dimension() == 3) {
            this->mesh->GetElementFaces(id, facesIds, _);
        }

        //Not using make_unique, because Element constructor is private.
        std::unique_ptr<Element> element(new Element(id, this->getFacesFromIds(facesIds)));
        return element;
    }

    std::unique_ptr<Point> MfemMesh::createPointFromId(int id) const { //TODO refactor this, dimension specific
        double coords[2];
        mesh->GetNode(id, coords);
        return std::make_unique<Point>(coords[0], coords[1]);
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

    Element *MfemMesh::getFaceAdjacentElement(const Face *face, const Vector &direction) const {
        int elementA, elementB;
        this->mesh->GetFaceElements(face->getId(), &elementA, &elementB);

        auto normal = face->getNormal();

        if (normal * direction < 0) {
            return this->getElementFromId(elementA);
        } else {
            return this->getElementFromId(elementB);
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

    std::vector<Point *> MfemMesh::genInnerPoints() {
        std::vector<Point *> result;
        std::set<Point *> boundaryPoints;
        for (const auto &face : this->boundaryFaces) {
            for (const auto &point : face->getPoints()) {
                boundaryPoints.insert(point);
            }
        }
        for (const auto &point : this->points) {
            if (boundaryPoints.find(point.get()) == boundaryPoints.end()) {
                result.emplace_back(point.get());
            }
        }
        return result;
    }

    std::vector<Point *> MfemMesh::getInnerPoints() const {
        return this->innerPoints;
    }

    std::vector<Element *> MfemMesh::getPointAdjacentElements(const Point *point) const {
        //TODO precalculate this
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
        std::transform(this->points.begin(), this->points.end(), std::back_inserter(result), [](const auto &point) {
            return point.get();
        });
        return result;
    }

    mfem::Mesh *MfemMesh::getMfemMesh() {
        return this->mesh;
    }

    std::vector<Element *> MfemMesh::getElements() const {
        std::vector<Element *> result;
        std::transform(
                this->elements.begin(), this->elements.end(), std::back_inserter(result),
                [](const auto &element) {
                    return element.get();
                }
        );
        return result;
    }
}
