#ifndef RAYTRACER_GRID_FUNCTION_H
#define RAYTRACER_GRID_FUNCTION_H

#include <mfem.hpp>
#include <set>
#include <functional>
#include "geometry_primitives.h"
#include "mesh.h"

namespace raytracer {
    /**
     * Abstract interface. To obey this MeshFunction interface getValue, setValue and addValue methods must be
     * implemented.
     */
    class MeshFunc {
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

        virtual size_t length() const = 0;

        /** Unique pointer to MeshFunction */
        using Ptr = std::unique_ptr<MeshFunc>;

        /** Function able taking an element and returning a value */
        using Transform = std::function<double(Element)>;
    };

    /**
     * A wrapper aggregating mfem::FiniteElementSpace and MfemMesh
     */
    class MfemSpace {
    public:
        /**
         * Override this
         * @return
         */
        virtual const MfemMesh &getMesh() const = 0;

        /**
         * Override this
         * @return
         */
        virtual mfem::FiniteElementSpace &getSpace() = 0;
    };

    /**
     * Wrapper around mfem L2_FECollection and FiniteElementSpace
     */
    class MfemL20Space : public MfemSpace {
    public:
        /**
         * Initialize the space using an mfem mesh
         * @param mesh
         */
        explicit MfemL20Space(const MfemMesh &mesh) :
                mesh(mesh), l2FiniteElementSpace(mesh.getMfemMesh(), &l2FiniteElementCollection) {}

        /**
         * Obtain const ref to the mesh
         * @return
         */
        const MfemMesh &getMesh() const override;

        /**
         * Obtain ref to the space
         * @return
         */
        mfem::FiniteElementSpace &getSpace() override;

    private:
        mfem::L2_FECollection l2FiniteElementCollection{0, 2};
        const MfemMesh &mesh;
        mfem::FiniteElementSpace l2FiniteElementSpace;
    };

    class MfemQuadFuncWrapper : public MeshFunc {
    public:
        MfemQuadFuncWrapper() = default;

        MfemQuadFuncWrapper(mfem::QuadratureFunction *quadFunc, const mfem::IntegrationRule *integRule) :
                quadFunc(quadFunc), integRule(integRule) {}

        double getValue(const Element &element) const override {
            mfem::Vector values;
            quadFunc->GetElementValues(element.getId(), values);
            return values.Sum() / values.Size();
        }

        void setValue(const Element &element, double value) override {
            mfem::Vector values;
            quadFunc->GetElementValues(element.getId(), values);

            for (int q = 0; q < integRule->GetNPoints(); q++) {
                values(q) = value;
            }
        }

        void addValue(const Element &element, double value) override {
            mfem::Vector values;
            quadFunc->GetElementValues(element.getId(), values);

            for (int q = 0; q < integRule->GetNPoints(); q++) {
                values(q) += value;
            }
        }

        size_t length() const override {
            return this->quadFunc->Size() / integRule->GetNPoints();
        }

    private:
        mfem::QuadratureFunction *quadFunc{};
        const mfem::IntegrationRule *integRule{};
    };


    class MfemVectorWrapper : public MeshFunc {
    public:
        MfemVectorWrapper() = default;

        explicit MfemVectorWrapper(mfem::Vector *vec) : vec(vec) {}

        double getValue(const Element &element) const override {
            return (*vec)(element.getId());
        }

        void setValue(const Element &element, double value) override {
            (*vec)(element.getId()) = value;
        }

        void addValue(const Element &element, double value) override {
            (*vec)(element.getId()) += value;
        }

        size_t length() const override {
            return vec->Size();
        }

    private:
        mfem::Vector *vec{};
    };

    class SimpleFunc : public MeshFunc {
    public:
        SimpleFunc() = default;

        explicit SimpleFunc(const Mesh *mesh) :
                values(mesh->getElements().size(), 0) {
        }

        double getValue(const Element &element) const override {
            return values[element.getId()];
        }

        void setValue(const Element &element, double value) override {
            values[element.getId()] = value;
        }

        void addValue(const Element &element, double value) override {
            values[element.getId()] += value;
        }

        size_t length() const override {
            return values.size();
        }

    private:
        std::vector<double> values{0};
    };

    /**
     * Discrete function what has constant values at Mesh elements.
     * Provides a way to query the GridFunction given an element.
     */
    class MfemMeshFunction : public MeshFunc {
    public:

        /**
         * Create the MfemMeshFunction from a mfem::GridFunction and mfem::FiniteElementSpace. Does not own the GridFunction
         * @param gridFunction a mutable reference will be kept.
         * @param mfemSpace const reference will be kept - caution: L2 space is expected!
         */
        MfemMeshFunction(MfemSpace &mfemSpace, mfem::GridFunction &gridFunction);

        /**
         * Sample an analytic function at element centroid and set the internal mfem GridFunction accordingly
         * @param mfemSpace
         * @param func
         */
        explicit MfemMeshFunction(MfemSpace &mfemSpace, const std::function<double(Point)> &func);

        explicit MfemMeshFunction(MfemSpace &mfemSpace, const std::function<double(Element)> &func);

        mfem::GridFunction *getGF() {
            return &this->mfemGridFunction;
        }

        /**
         * By default initialize to 0
         * @param mfemSpace
         */
        explicit MfemMeshFunction(MfemSpace &mfemSpace) :
                mfemGridFunction(&mfemSpace.getSpace()),
                gridFunction(mfemGridFunction),
                mfemSpace(mfemSpace) {
            this->init();
            mfemGridFunction = 0;
        }

        /**
         * Construct using a serializes mesh function from a given stream
         * @param mfemSpace
         * @param is
         */
        explicit MfemMeshFunction(MfemSpace &mfemSpace, std::istream &is);

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

        size_t length() const override {
            return eleTrueVecInd.size();
        }

        /**
         * Output a string representation of MfemMeshFunction
         * @param os
         * @param meshFunction
         * @return
         */
        friend std::ostream &operator<<(std::ostream &os, const MfemMeshFunction &meshFunction);


    private:
        mfem::GridFunction mfemGridFunction;
        mfem::GridFunction &gridFunction;
        MfemSpace &mfemSpace;
        std::vector<int> eleTrueVecInd;

        double &get(const Element &element);

        void init();

        std::vector<int> getEleTrueVecInd();
    };

    std::ostream &operator<<(std::ostream &os, const MfemMeshFunction &meshFunction);

    void divideByVolume(const raytracer::MfemMesh &mesh, raytracer::MeshFunc &func);
}


#endif //RAYTRACER_GRID_FUNCTION_H
