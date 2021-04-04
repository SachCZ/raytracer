#include "mesh_function.h"

namespace raytracer {
    double MfemMeshFunction::getValue(const Element &element) const {
        return const_cast<MfemMeshFunction *>(this)->get(element);
    }

    void MfemMeshFunction::setValue(const Element &element, double value) {
        this->get(element) = value;
    }

    void MfemMeshFunction::addValue(const Element &element, double value) {
        this->get(element) += value;
    }

    double &MfemMeshFunction::get(const Element &element) {
        auto &trueVector = gridFunction.GetTrueVector();
        return trueVector[eleTrueVecInd[element.getId()]];
    }

    MfemMeshFunction::MfemMeshFunction(MfemSpace &mfemSpace, mfem::GridFunction &gridFunction) :
            gridFunction(gridFunction),
            mfemSpace(mfemSpace) {
        this->init();
    }

    std::ostream &operator<<(std::ostream &os, const MfemMeshFunction &meshFunction) {
        meshFunction.gridFunction.Save(os);
        return os;
    }

    void divideByVolume(const MfemMesh &mesh, MeshFunc &func) {
        for (auto element : mesh.getElements()){
            auto volume = raytracer::getElementVolume(*element);
            func.setValue(*element, func.getValue(*element) / volume);
        }
    }

    MfemMeshFunction::MfemMeshFunction(MfemSpace &mfemSpace, const std::function<double(Point)> &func) :
            mfemGridFunction(&mfemSpace.getSpace()),
            gridFunction(mfemGridFunction),
            mfemSpace(mfemSpace) {
        this->init();
        for (const auto &element : mfemSpace.getMesh().getElements()) {
            this->get(*element) = func(getElementCentroid(*element));
        }
    }

    MfemMeshFunction::MfemMeshFunction(MfemSpace &mfemSpace, const std::function<double(Element)> &func) :
            mfemGridFunction(&mfemSpace.getSpace()),
            gridFunction(mfemGridFunction),
            mfemSpace(mfemSpace) {
        this->init();
        for (const auto &element : mfemSpace.getMesh().getElements()) {
            this->get(*element) = func(*element);
        }
    }

    void MfemMeshFunction::init() {
        gridFunction.SetTrueVector();
        eleTrueVecInd = getEleTrueVecInd();
    }

    MfemMeshFunction::MfemMeshFunction(MfemSpace &mfemSpace, std::istream &is) :
            mfemGridFunction(mfemSpace.getMesh().getMfemMesh(), is),
            gridFunction(mfemGridFunction),
            mfemSpace(mfemSpace) {
        this->init();
    }

    std::vector<int> MfemMeshFunction::getEleTrueVecInd() {
        const auto &elements = mfemSpace.getMesh().getElements();

        int resultSize = 0;
        std::for_each(elements.begin(), elements.end(),
           [&resultSize](Element *element) {
            if (element->getId() > resultSize){
                resultSize = element->getId();
            }
        });

        std::vector<int> result(resultSize + 1, -1);

        for (Element *element : elements) {
            mfem::Array<int> vdofs;
            mfemSpace.getSpace().GetElementInteriorDofs(element->getId(), vdofs);
            result[element->getId()] = vdofs[0];
        }
        return result;
    }

    mfem::FiniteElementSpace &MfemL20Space::getSpace() {
        return this->l2FiniteElementSpace;
    }

    const MfemMesh &MfemL20Space::getMesh() const {
        return this->mesh;
    }
}
