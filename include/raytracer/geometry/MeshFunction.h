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
                mfem::Array<int> vdofs;
                finiteElementSpace.GetElementInteriorDofs(element.id, vdofs);
                auto &trueVector = gridFunction.GetTrueVector();
                return trueVector[vdofs[0]];
            }

        private:
            mfem::GridFunction& gridFunction;
            const mfem::FiniteElementSpace& finiteElementSpace;
        };
    }
}


#endif //RAYTRACER_GRID_FUNCTION_H
