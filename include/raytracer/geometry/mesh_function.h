#ifndef RAYTRACER_GRID_FUNCTION_H
#define RAYTRACER_GRID_FUNCTION_H

#include <mfem.hpp>
#include <set>
#include <functional>
#include "geometry_primitives.h"
#include "mesh.h"

namespace raytracer {
    /**
     * \addtogroup api
     * @{
     */

    /**
     * Abstract interface. To obey this MeshFunction interface getValue, setValue and addValue methods must be
     * implemented.
     */
    class MeshFunction {
    public:
        /**
         * Override this.
         * @return value of MeshFunction at element
         */
        virtual double getValue(const Element &) const = 0;

        /**
         * Override this.
         * @param value value to be set at element
         */
        virtual void setValue(const Element &, double value) = 0;

        /**
         * Override this.
         * @param value to be added to current value at element.
         */
        virtual void addValue(const Element &, double value) = 0;
    };

    /**
     * Discrete function what has constant values at Mesh elements.
     * Provides a way to query the GridFunction given an element.
     */
    class MfemMeshFunction : public MeshFunction {
    public:

        /**
         * Create the MeshFunction from a mfem::GridFunction and mfem::FiniteElementSpace.
         * @param gridFunction a mutable reference will be kept.
         * @param finiteElementSpace const reference will be kept - caution: L2 space is expected!
         */
        MfemMeshFunction(
                mfem::GridFunction &gridFunction,
                const mfem::FiniteElementSpace &finiteElementSpace);

        explicit MfemMeshFunction(mfem::FiniteElementSpace &finiteElementSpace);

        /**
         * Get a value at Element.
         * @param element
         * @return the value at Element
         */
        double getValue(const Element &element) const override;

        /**
         * Set a value at Element.
         * @param element
         * @param value
         */
        void setValue(const Element &element, double value) override;

        /**
         * Add a value to existing value at Element.
         * @param element
         * @param value
         */
        void addValue(const Element &element, double value) override;

        friend std::ostream& operator<<(std::ostream& os, const MfemMeshFunction& meshFunction);

        void setUsingFunction(const Mesh& mesh, const std::function<double(Point)>& func){
            for (const auto& element : mesh.getElements()){
                this->setValue(*element, func(getElementCentroid(*element)));
            }
        }

    private:
        mfem::GridFunction mfemGridFunction;
        mfem::GridFunction &gridFunction;
        const mfem::FiniteElementSpace &finiteElementSpace;

        double &get(const Element &element);

        void init();
    };

    std::ostream &operator<<(std::ostream &os, const MfemMeshFunction &meshFunction);

    /**
     * @}
     */
}


#endif //RAYTRACER_GRID_FUNCTION_H
