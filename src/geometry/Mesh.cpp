#include <GeometryFunctions.h>
#include "Mesh.h"
#include "Element.h"
#include <cmath>
#include <algorithm>

namespace raytracer {
    namespace geometry {
        Element * Mesh::getAdjacentElement(const Face *face, const raytracer::geometry::HalfLine &orientation) const {
            auto boundaryPoint = face->isBoundary(orientation.point);
            if (!boundaryPoint){
                return this->getFaceAdjacentElement(face, orientation.direction);
            } else {
                //Diagonal edge case
                auto candidateElements = this->getAdjacentElements(boundaryPoint);
                for (const auto& element : candidateElements){
                    auto _faces = element->getFaces();
                    std::vector<Face*> oppositeFaces;
                    std::copy_if(_faces.begin(), _faces.end(), std::back_inserter(oppositeFaces),
                            [boundaryPoint](const Face* face){
                        const auto& _points =  face->getPoints();
                        return !(_points[0] == boundaryPoint || _points[1] == boundaryPoint);
                    });

                    if (findClosestIntersection(orientation, oppositeFaces)){
                        return element;
                    }
                }
                return nullptr;
            }
        }

        std::vector<Face *> Mesh::getBoundary() const {
            return this->boundaryFaces;
        }

        Mesh::Mesh(mfem::Mesh *mesh): mesh(mesh), vertexToElementTable(mesh->GetVertexToElementTable()) {
            this->points = this->genPoints();
            this->faces = this->genFaces();
            this->elements = this->genElements();

            this->boundaryFaces = Mesh::genBoundaryFaces();
        }

        std::unique_ptr <Element> Mesh::createElementFromId(int id) const {
            if (id == -1) return nullptr;

            mfem::Array<int> facesIds;
            mfem::Array<int> _;

            if (this->mesh->Dimension() == 2) {
                this->mesh->GetElementEdges(id, facesIds, _);
            } else if (this->mesh->Dimension() == 3){
                this->mesh->GetElementFaces(id, facesIds, _);
            }

            //Not using make_unique, because Element constructor is private.
            std::unique_ptr<Element> element(new Element(id, this->getFacesFromIds(facesIds)));
            return element;
        }

        std::unique_ptr<Point> Mesh::createPointFromId(int id) const { //TODO refactor this, dimension specific
            double coords[2];
            mesh->GetNode(id, coords);
            return std::make_unique<Point>(coords[0], coords[1]);
        }

        std::vector<Point *> Mesh::getPointsFromIds(const mfem::Array<int> &ids) const {
            std::vector<Point*> result;
            for (auto id : ids) {
                result.emplace_back(this->points[id].get());
            }
            return result;
        }

        std::unique_ptr<Face> Mesh::createFaceFromId(int id) const {
            mfem::Array<int> faceVerticesIds;
            this->mesh->GetFaceVertices(id, faceVerticesIds);

            std::unique_ptr<Face> face(new Face(id, this->getPointsFromIds(faceVerticesIds)));
            return face;
        }

        std::vector<Face *> Mesh::genBoundaryFaces() {
            std::vector<Face*> result;
            for (int i = 0; i < this->mesh->GetNumFaces(); ++i){
                int id1, id2;
                this->mesh->GetFaceElements(i, &id1, &id2);
                if (id1 == -1 || id2 == -1){
                    result.emplace_back(this->faces[i].get());
                }
            }
            return result;
        }

        Element *Mesh::getElementFromId(int id) const {
            if (id < 0) return nullptr;
            else return this->elements[id].get();
        }

        std::vector<std::unique_ptr<Point>> Mesh::genPoints() {
            std::vector<std::unique_ptr<Point>> result;
            result.reserve(mesh->GetNV());
            for (int id = 0; id < mesh->GetNV(); ++id){
                result.emplace_back(this->createPointFromId(id));
            }
            return result;
        }

        std::vector<std::unique_ptr<Face>> Mesh::genFaces() {
            std::vector<std::unique_ptr<Face>> result;
            int facesCount = this->mesh->Dimension() == 2 ? this->mesh->GetNEdges() : this->mesh->GetNFaces();

            result.reserve(facesCount);
            for (int id = 0; id < facesCount; ++id) {
                result.emplace_back(this->createFaceFromId(id));
            }
            return result;
        }

        std::vector<std::unique_ptr<Element>> Mesh::genElements() {
            std::vector<std::unique_ptr<Element>> result;

            result.reserve(mesh->GetNE());
            for (int id = 0; id < mesh->GetNE(); ++id) {
                result.emplace_back(this->createElementFromId(id));
            }
            return result;
        }

        std::vector<Face *> Mesh::getFacesFromIds(const mfem::Array<int> &ids) const {
            std::vector<Face *> result;
            size_t size = std::distance(ids.begin(), ids.end());
            result.reserve(size);
            for (auto id : ids) {
                result.emplace_back(this->faces[id].get());
            }
            return result;
        }

        Element *Mesh::getFaceAdjacentElement(const Face *face, const Vector &direction) const {
            int elementA, elementB;
            this->mesh->GetFaceElements(face->getId(), &elementA, &elementB);

            auto normal = face->getNormal();

            if (normal * direction < 0) {
                return this->getElementFromId(elementA);
            } else {
                return this->getElementFromId(elementB);
            }
        }

        std::unique_ptr<mfem::Mesh> constructRectangleMesh(DiscreteLine sideA, DiscreteLine sideB) {
            return std::make_unique<mfem::Mesh>(
                    sideA.segmentCount,
                    sideB.segmentCount,
                    mfem::Element::Type::QUADRILATERAL,
                    true,
                    sideA.length,
                    sideB.length,
                    true);
        }
    }
}
