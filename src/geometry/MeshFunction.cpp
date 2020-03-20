#include "MeshFunction.h"

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
        mfem::Array<int> vdofs;
        finiteElementSpace.GetElementInteriorDofs(element.getId(), vdofs);
        auto &trueVector = gridFunction.GetTrueVector();
        return trueVector[vdofs[0]];
    }

    MfemMeshFunction::MfemMeshFunction(mfem::GridFunction &gridFunction,
                                       const mfem::FiniteElementSpace &finiteElementSpace) :
            gridFunction(gridFunction),
            finiteElementSpace(finiteElementSpace) {
        gridFunction.SetTrueVector();
    }
}
