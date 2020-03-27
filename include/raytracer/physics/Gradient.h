#ifndef RAYTRACER_GRADIENT_H
#define RAYTRACER_GRADIENT_H

#include "Matrix2D.h"
#include "Vector.h"
#include "Intersection.h"
#include "mfem.hpp"
#include "Point.h"
#include "MeshFunction.h"
#include "Mesh.h"

namespace raytracer {
    /**
     * \addtogroup gradients
     * @{
     */

    /**
     * Abstract interface to provide a GradientCalculator. To obey this interface implement the
     * get method.
     */
    class Gradient {
    public:
        /**
         * Override this.
         * @return the gradient at given intersection PointOnFace.
         */
        virtual Vector get(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const = 0;
    };

    /**
     * GradientCalculator that returns a constant Vector no matter what.
     */
    class ConstantGradient : public Gradient {
    public:
        /**
         * Constructor that takes the Vector that will be returned every time as parameter.
         * @param gradient - the vector to be returned
         */
        explicit ConstantGradient(const Vector &gradient);

        /**
         * Returns always the same Vector given at construction.
         * @return vector gradient.
         */
        Vector get(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const override;

    private:
        const Vector gradient;
    };

    /**
     * GradientCalculator that stores a density grid function defined in H1 space obtained from function given
     * if L2. When get is called the gradient of this function is calculated at given point.
     *
     * @warning
     * Please note that updateDensity() must be called before calling get().
     */
    class H1Gradient : public Gradient {
    public:
        /**
         * Constructor that expects the l2 and h1 spaces. L2 is the space in which density is usually defined
         * and H1 is the space into which the density is transformed to be able to evaluate gradient.
         * @param l2Space
         * @param h1Space
         */
        H1Gradient(
                mfem::FiniteElementSpace &l2Space,
                mfem::FiniteElementSpace &h1Space
        );

        /**
         * Return the value of gradient at the intersection point.
         *
         * @param pointOnFace
         * @param previousElement
         * @param nextElement
         * @return gradient at given point
         */
        Vector get(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const override;


        /**
         * Update the density from which the gradient is calculated. The density should be a function in L2 space.
         * @param density defined over L2
         */
        void updateDensity(mfem::GridFunction &density);

    private:
        mfem::FiniteElementSpace &l2Space;
        mfem::FiniteElementSpace &h1Space;
        mfem::GridFunction _density;

        Vector getGradientAt(const Element &element, const Point &point) const;

        mfem::GridFunction projectL2toH1(const mfem::GridFunction &function);
    };

    /**
     * GradientCalculator that returns the normal to the face as Gradient.
     */
    class StepGradient : public Gradient {
    public:
        /**
         * Get gradient as a normal to the Face given by PointOnFace.
         *
         * @param pointOnFace
         * @return normal as gradient
         */
        Vector get(
                const PointOnFace &pointOnFace,
                const Element &,
                const Element &
        ) const override {
            //TODO fix direction
            return pointOnFace.face->getNormal();
        }
    };

    class LeastSquare : public Gradient {
    public:
        explicit LeastSquare(const Mesh &mesh, const MeshFunction &density) : mesh(mesh), density(density) {}

        Vector
        get(const PointOnFace &, const Element &previousElement, const Element &nextElement) const override {
            auto previousGradient = this->getValueAt(previousElement, mesh.getElementAdjacentElements(previousElement));
            auto nextGradient = this->getValueAt(nextElement, mesh.getElementAdjacentElements(nextElement));
            return 1.0 / (previousGradient.getNorm() + nextGradient.getNorm()) *
                   (previousGradient.getNorm() * previousGradient + nextGradient.getNorm() * nextGradient);
        }

    private:
        const Mesh &mesh;
        const MeshFunction &density;

        static std::array<double, 2> getCentroid(const Element &element) {
            //TODO Horribly terrible just for triangles

            auto points = element.getPoints();
            if (points.size() != 3) throw std::logic_error("Centroid only available for triangles.");

            return {(points[0]->x + points[1]->x + points[2]->x) / 3.0, (points[0]->y + points[1]->y + points[2]->y) / 3.0};
        }

        Vector getValueAt(const Element &element, const std::vector<Element *> &neighbours) const {
            Matrix2D A{};
            A.xx = getCoeffA(element, neighbours, 0, 0);
            A.xy = getCoeffA(element, neighbours, 0, 1);
            A.yx = getCoeffA(element, neighbours, 1, 0);
            A.yy = getCoeffA(element, neighbours, 1, 1);

            Vector b{};
            b.x = getCoeffB(element, neighbours, 0);
            b.y = getCoeffB(element, neighbours, 1);
            return solve(A, b);
        }

        static double getCoeffA(
                const Element &element,
                const std::vector<Element *> &neighbours,
                uint firstAxis, uint secondAxis
        ) {
            double sum = 0;
            auto eleCenter = getCentroid(element);
            for (const auto &neighbour : neighbours) {
                auto neighbourCenter = getCentroid(*neighbour);
                sum += (eleCenter[firstAxis] - neighbourCenter[firstAxis]) *
                       (eleCenter[secondAxis] - neighbourCenter[secondAxis]);
            }
            return 2 * sum;
        }

        double getCoeffB(const Element &element, const std::vector<Element *> &neighbours, uint axis) const {
            double sum = 0;
            auto eleCenter = getCentroid(element);
            for (const auto &neighbour : neighbours) {
                auto neighbourCenter = getCentroid(*neighbour);
                sum += (eleCenter[axis] - neighbourCenter[axis]) *
                       (density.getValue(element) - density.getValue(*neighbour));
            }
            return 2 * sum;
        }

        static Vector solve(Matrix2D A, Vector b) { //Crammer
            auto det = A.yy * A.xx - A.xy * A.yx;
            auto x = (b.x * A.yy - A.xy * b.y) / det;
            auto y = (A.xx*b.y - b.x * A.yx) / det;
            return {x, y};
        }
    };

    /**
     * @}
     */
}


#endif //RAYTRACER_GRADIENT_H
