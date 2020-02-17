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
                    mfem::GridFunction& gridFunction,
                    const mfem::FiniteElementSpace& finiteElementSpace):
            gridFunction(gridFunction),
            finiteElementSpace(finiteElementSpace){
                if (!this->hasProperVdofs()){
                    throw std::logic_error("Elements in finite elements space must have exactly"
                                           " one internal degree of freedom to be used with MeshFunction!");
                }
                if (!this->orderIs(0)) throw std::logic_error("Zeroth order elements are expected!");
                gridFunction.SetTrueVector();
            }

            /**
             * Get a mutable reference of underlying mfem::GridFunction based on an element.
             * @param element
             * @return the value of GridFunction at the element true dof.
             */
            double& operator[](const Element& element){
                return const_cast<double&>(const_cast<const MeshFunction*>(this)->get(element)); }

            /**
             * Get a const reference of underlying mfem::GridFunction based on an element.
             * @param element
             * @return the value of GridFunction at the element true dof.
             */
            const double& operator[](const Element& element) const {
                return this->get(element);
            }


            Vector getGradient(const Element& element){
                return {}; //TODO use the Method suggested by Kucharik
            }

        private:
            mfem::GridFunction& gridFunction;
            const mfem::FiniteElementSpace& finiteElementSpace;

            bool orderIs(int order){
                for (int i = 0; i < finiteElementSpace.GetNE(); ++i){
                    if (finiteElementSpace.GetOrder(i) != order) return false;
                }
                return true;
            }

            bool hasProperVdofs(){
                for (int i = 0; i < finiteElementSpace.GetNE(); ++i){
                    mfem::Array<int> vdofs;
                    finiteElementSpace.GetElementInteriorDofs(i, vdofs);
                    if (vdofs.Size() != 1) return false;
                }
                return true;
            }

            const double& get(const Element& element) const {
                mfem::Array<int> vdofs;
                mfem::Vector result;
                finiteElementSpace.GetElementVDofs(element.id, vdofs);
                gridFunction.GetSubVector(vdofs, result);
                return result[0];
            }
        };
    }
}


#endif //RAYTRACER_GRID_FUNCTION_H
