#ifndef RAYTRACER_GRID_FUNCTION_H
#define RAYTRACER_GRID_FUNCTION_H

#include <mfem.hpp>
#include <Element.h>

namespace raytracer {
    namespace geometry {

        /**
         * Wrapper class around mfem::GridFunction.
         * Provides a way to query the GridFunction given an element.
         */

        class MeshFunction {
        public:

            /**
             * Create the MeshFunction from a mfem::GridFunction and mfem::FiniteElementSpace.
             * @param gridFunction a mutable reference will be kept.
             * @param finiteElementSpace const reference will be kept - caution: L2 space is expected!
             */
            explicit MeshFunction(
                    mfem::GridFunction& gridFunction,
                    const mfem::FiniteElementSpace& finiteElementSpace):
            gridFunction(gridFunction),
            finiteElementSpace(finiteElementSpace){
                gridFunction.SetTrueVector();
            }

            /**
             * Get a mutable reference of underlying mfem::GridFunction based on an element.
             * @param element
             * @return the value of GridFunction at the element true dof.
             */
            double& operator[](const Element& element){
                return const_cast<double&>(const_cast<const MeshFunction*>(this)->get(element));
            }

            /**
             * Get a const reference of underlying mfem::GridFunction based on an element.
             * @param element
             * @return the value of GridFunction at the element true dof.
             */
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
