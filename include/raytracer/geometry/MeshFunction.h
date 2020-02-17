#ifndef RAYTRACER_GRID_FUNCTION_H
#define RAYTRACER_GRID_FUNCTION_H

#include <mfem.hpp>
#include <Element.h>
#include <stdexcept>

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
                    mfem::GridFunction &gridFunction,
                    const mfem::FiniteElementSpace &finiteElementSpace) :
                    gridFunction(gridFunction),
                    finiteElementSpace(finiteElementSpace) {
                gridFunction.SetTrueVector();
            }

            /**
             * Get a value of mfem::GridFunction based on an element.
             * @param element
             * @return the value of GridFunction at the element true dof.
             */
            double getValue(const Element &element) const {
                return const_cast<MeshFunction*>(this)->get(element);
            }

            /**
             * Set a value of mfem::GridFunction based on an element.
             * @param element
             * @param value
             */
            void setValue(const Element &element, double value) {
                this->get(element) = value;
            }

            /**
             * Add a value to existing value of mfem::GridFunction based on an element.
             * @param element
             * @param value
             */
            void addValue(const Element &element, double value) {
                this->get(element) += value;
            }

        private:
            mfem::GridFunction &gridFunction;
            const mfem::FiniteElementSpace &finiteElementSpace;

            double &get(const Element &element) {
                mfem::Array<int> vdofs;
                finiteElementSpace.GetElementInteriorDofs(element.id, vdofs);
                auto &trueVector = gridFunction.GetTrueVector();
                return trueVector[vdofs[0]];
            }
        };
    }
}


#endif //RAYTRACER_GRID_FUNCTION_H
