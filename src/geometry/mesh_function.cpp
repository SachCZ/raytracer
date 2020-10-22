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

    MfemMeshFunction::MfemMeshFunction(MfemSpace &mfemSpace, const std::function<double(Point)> &func) :
            mfemGridFunction(&mfemSpace.getSpace()),
            gridFunction(mfemGridFunction),
            mfemSpace(mfemSpace) {
        this->init();
        this->setUsingFunction(mfemSpace.getMesh(), func);
    }

    MeshFunc::Ptr MfemMeshFunction::calcTransformed(const MeshFunc::Transform & func) const {
        auto result = std::make_unique<MfemMeshFunction>(this->mfemSpace);
        for (Element *element : mfemSpace.getMesh().getElements()) {
            result->setValue(*element, func(*element));
        }
        return result;
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
        std::vector<int> ids;
        const auto &elements = mfemSpace.getMesh().getElements();
        ids.reserve(elements.size());
        std::transform(elements.begin(), elements.end(), std::back_inserter(ids),
           [](Element *element) {
               return element->getId();
           });
        int resultSize = *std::max_element(ids.begin(), ids.end());
        std::vector<int> result(resultSize);

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
