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
                                           " one internal degree of freedom to be used with MeshFunction");
                }
                if (this->orderIs(0)){
                    this->isZerothOrder = true;
                }
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
                if (this->isZerothOrder) throw std::logic_error("Gradient on zeroth order finite"
                                                                "element space will always be zero!");
                mfem::Vector result;
                auto isoparametricTransformation = this->finiteElementSpace.GetElementTransformation(element.id);
                this->gridFunction.GetGradient(*isoparametricTransformation, result);
                return {result[0], result[1]};
            }

        private:
            mfem::GridFunction& gridFunction;
            const mfem::FiniteElementSpace& finiteElementSpace;
            bool isZerothOrder = false;

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
                finiteElementSpace.GetElementInteriorDofs(element.id, vdofs);
                auto &trueVector = gridFunction.GetTrueVector();
                return trueVector[vdofs[0]];
            }
        };
    }
}


#endif //RAYTRACER_GRID_FUNCTION_H
