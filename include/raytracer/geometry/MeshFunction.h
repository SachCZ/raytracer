#ifndef RAYTRACER_GRID_FUNCTION_H
#define RAYTRACER_GRID_FUNCTION_H

#include <mfem.hpp>
#include <Element.h>

namespace raytracer {
    namespace geometry {

        /**
         * Abstract interface. To obey this MeshFunction interface getValue, setValue and addValue methods must be
         * implemented.
         */
        class MeshFunction {
        public:
            virtual double getValue(const Element&) const = 0;
            virtual void setValue(const Element&, double value) = 0;
            virtual void addValue(const Element&, double value) = 0;
        };

        /**
         * Wrapper class around mfem::GridFunction.
         * Provides a way to query the GridFunction given an element.
         */
        class MfemMeshFunction : public MeshFunction {
        public:

            /**
             * Create the MeshFunction from a mfem::GridFunction and mfem::FiniteElementSpace.
             * @param gridFunction a mutable reference will be kept.
             * @param finiteElementSpace const reference will be kept - caution: L2 space is expected!
             */
            explicit MfemMeshFunction(
                    mfem::GridFunction &gridFunction,
                    const mfem::FiniteElementSpace &finiteElementSpace);

            /**
             * Get a value of mfem::GridFunction based on an element.
             * @param element
             * @return the value of GridFunction at the element true dof.
             */
            double getValue(const Element &element) const override;

            /**
             * Set a value of mfem::GridFunction based on an element.
             * @param element
             * @param value
             */
            void setValue(const Element &element, double value) override;

            /**
             * Add a value to existing value of mfem::GridFunction based on an element.
             * @param element
             * @param value
             */
            void addValue(const Element &element, double value) override;

        private:
            mfem::GridFunction &gridFunction;
            const mfem::FiniteElementSpace &finiteElementSpace;

            double &get(const Element &element);
        };
    }
}


#endif //RAYTRACER_GRID_FUNCTION_H
