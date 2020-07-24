#include "Gradient.h"

namespace raytracer {

    ConstantGradient::ConstantGradient(const Vector &gradient) : gradient(gradient) {}

    Vector ConstantGradient::get(
            const PointOnFace &,
            const Element &,
            const Element &
    ) const {
        return this->gradient;
    }

    H1Gradient::H1Gradient(
            mfem::FiniteElementSpace &l2Space,
            mfem::FiniteElementSpace &h1Space
    ) :
            l2Space(l2Space), h1Space(h1Space), _density(&h1Space) {}

    Vector H1Gradient::get(
            const PointOnFace &pointOnFace,
            const Element &previousElement,
            const Element &nextElement
    ) const {
        auto point = pointOnFace.point;
        auto previousGradient = this->getGradientAt(previousElement, point);
        auto nextGradient = this->getGradientAt(nextElement, point);
        return 0.5 * (previousGradient + nextGradient); //TODO look into this
    }

    void H1Gradient::updateDensity(mfem::GridFunction &density) {
        this->_density = this->projectL2toH1(density);
    }

    Vector
    H1Gradient::getGradientAt(const Element &element, const Point &point) const {
        mfem::Vector result(2);
        mfem::IntegrationPoint integrationPoint{};
        integrationPoint.Set2(point.x, point.y);

        auto transformation = this->h1Space.GetElementTransformation(element.getId());
        transformation->SetIntPoint(&integrationPoint);
        this->_density.GetGradient(*transformation, result);

        return {result[0], result[1]};
    }

    mfem::GridFunction H1Gradient::projectL2toH1(const mfem::GridFunction &function) {
        mfem::BilinearForm A(&h1Space);
        A.AddDomainIntegrator(new mfem::MassIntegrator);
        A.Assemble();
        A.Finalize();

        mfem::MixedBilinearForm B(&l2Space, &h1Space);
        B.AddDomainIntegrator(new mfem::MixedScalarMassIntegrator);
        B.Assemble();
        B.Finalize();

        mfem::LinearForm b(&h1Space);
        B.Mult(function, b);

        mfem::GSSmoother smoother(A.SpMat());

        mfem::GridFunction result(&h1Space);
        result = 0;
        mfem::PCG(A, smoother, b, result);

        return result;
    }

    LeastSquare::LeastSquare(const Mesh &mesh, const MeshFunction &density) : mesh(mesh), density(density) {}

    Vector LeastSquare::get(const PointOnFace &, const Element &previousElement, const Element &nextElement) const {
        auto previousGradient = this->getValueAt(previousElement, mesh.getElementAdjacentElements(previousElement));
        auto nextGradient = this->getValueAt(nextElement, mesh.getElementAdjacentElements(nextElement));
        return normWeightedAverage({previousGradient, nextGradient});
    }

    Vector LeastSquare::normWeightedAverage(const std::vector<Vector> &vectors) {
        Vector numerator{};
        double denominator = 0;
        for (const auto& vector : vectors){
            if (vector.getNorm() == 0) continue;
            numerator = numerator + vector.getNorm() * vector;
            denominator += vector.getNorm();
        }
        return denominator == 0 ? Vector(0,0) : 1.0 / denominator * numerator;
    }

    std::array<double, 2> LeastSquare::getCentroid(const Element &element) {
        //TODO Horribly terrible just for triangles

        auto points = element.getPoints();
        if (points.size() != 3) throw std::logic_error("Centroid only available for triangles.");

        return {(points[0]->x + points[1]->x + points[2]->x) / 3.0, (points[0]->y + points[1]->y + points[2]->y) / 3.0};
    }

    Vector LeastSquare::getValueAt(const Element &element, const std::vector<Element *> &neighbours) const {
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

    double LeastSquare::getCoeffA(const Element &element, const std::vector<Element *> &neighbours, uint firstAxis,
                                  uint secondAxis) {
        double sum = 0;
        auto eleCenter = getCentroid(element);
        for (const auto &neighbour : neighbours) {
            auto neighbourCenter = getCentroid(*neighbour);
            sum += (eleCenter[firstAxis] - neighbourCenter[firstAxis]) *
                   (eleCenter[secondAxis] - neighbourCenter[secondAxis]);
        }
        return 2 * sum;
    }

    double LeastSquare::getCoeffB(const Element &element, const std::vector<Element *> &neighbours, uint axis) const {
        double sum = 0;
        auto eleCenter = getCentroid(element);
        for (const auto &neighbour : neighbours) {
            auto neighbourCenter = getCentroid(*neighbour);
            sum += (eleCenter[axis] - neighbourCenter[axis]) *
                   (density.getValue(element) - density.getValue(*neighbour));
        }
        return 2 * sum;
    }

    Vector LeastSquare::solve(Matrix2D A, Vector b) { //Crammer
        auto det = A.yy * A.xx - A.xy * A.yx;
        if (det == 0) return {0, 0};
        auto x = (b.x * A.yy - A.xy * b.y) / det;
        auto y = (A.xx*b.y - b.x * A.yx) / det;
        return {x, y};
    }

    Vector NormalGradient::get(const PointOnFace &pointOnFace, const Element &previousElement,
                               const Element &nextElement) const {
        auto previousDensity = this->density.getValue(previousElement);
        auto nextDensity = this->density.getValue(nextElement);
        double factor = previousDensity > nextDensity ? -1 : 1;
        auto normal = pointOnFace.face->getNormal();
        return factor / normal.getNorm() * normal;
    }
}

