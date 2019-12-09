#ifndef RAYTRACER_GRID_FUNCTION_H
#define RAYTRACER_GRID_FUNCTION_H

#include <mfem.hpp>
#include <Element.h>

namespace raytracer {
    namespace geometry {
        class MeshFunction {
        public:
            explicit MeshFunction(
                    mfem::GridFunction& gridFunction,
                    const mfem::FiniteElementSpace& finiteElementSpace):
            gridFunction(gridFunction),
            finiteElementSpace(finiteElementSpace){
                gridFunction.SetTrueVector();
            }

            double& operator[](const Element& element){
                return const_cast<double&>(const_cast<const MeshFunction*>(this)->get(element));
            }

            const double& operator[](const Element& element) const {
                return this->get(element);
            }

        private:
            mfem::GridFunction& gridFunction;
            const mfem::FiniteElementSpace& finiteElementSpace;

            const double& get(const Element& element) const {
                mfem::Array<int> vdofs;
                finiteElementSpace.GetElementInteriorDofs(element.id, vdofs);
                auto &trueVector = gridFunction.GetTrueVector();
                return trueVector[vdofs[0]];
            }
        };
    }
}


#endif //RAYTRACER_GRID_FUNCTION_H
