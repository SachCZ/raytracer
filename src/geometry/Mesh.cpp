#include "Mesh.h"
namespace raytracer {
    namespace geometry {
        std::unique_ptr <Element> Mesh::getAdjacentElement(const Face &face, const Vector &direction) const {
            int elementA, elementB;
            this->mesh->GetFaceElements(face.id, &elementA, &elementB);

            auto normal = face.getNormal();

            if (normal * direction < 0) {
                return this->getElementFromId(elementA);
            } else {
                return this->getElementFromId(elementB);
            }
        }

        std::vector <Face> Mesh::getBoundary() const {
            return this->getFacesFromIds(this->boundaryFacesIds);
        }

        Mesh::Mesh(DiscreteLine sideA, DiscreteLine sideB) {
            this->mesh = std::make_unique<mfem::Mesh>(
                    sideA.segmentCount,
                    sideB.segmentCount,
                    mfem::Element::Type::QUADRILATERAL,
                    true,
                    sideA.width,
                    sideB.width,
                    true);
            this->boundaryFacesIds = Mesh::genBoundaryFacesIds(this->mesh.get());
        }

        std::unique_ptr <Element> Mesh::getElementFromId(int id) const {
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

        Point Mesh::getPointFromId(int id) const { //TODO refactor this, dimension specific
            double coords[2];
            mesh->GetNode(id, coords);
            return {coords[0], coords[1]};
        }

        std::vector <Point> Mesh::getPointsFromIds(const mfem::Array<int> &ids) const {
            std::vector<Point> result;
            for (auto id : ids) {
                result.emplace_back(this->getPointFromId(id));
            }
            return result;
        }

        Face Mesh::getFaceFromId(int id) const {
            mfem::Array<int> faceVerticesIds;
            this->mesh->GetFaceVertices(id, faceVerticesIds);

            return Face(id, this->getPointsFromIds(faceVerticesIds));
        }

        std::vector<int> Mesh::genBoundaryFacesIds(const mfem::Mesh *_mesh) {
            std::vector<int> result;
            for (int i = 0; i < _mesh->GetNumFaces(); ++i){
                int id1, id2;
                _mesh->GetFaceElements(i, &id1, &id2);
                if (id1 == -1 || id2 == -1){
                    result.emplace_back(i);
                }
            }
            return result;
        }

    }
}
