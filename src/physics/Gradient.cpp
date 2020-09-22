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

        char vishost[] = "localhost";
        int visport = 19916;
        mfem::socketstream sol_sock(vishost, visport);
        mfem::socketstream sol_sock2(vishost, visport);
        sol_sock.precision(8);
        sol_sock2.precision(8);
        sol_sock << "solution\n" << *this->h1Space.GetMesh() << this->_density << std::flush;
        sol_sock2 << "solution\n" << *this->l2Space.GetMesh() << density << std::flush;
    }

    Vector
    H1Gradient::getGradientAt(const Element &element, const Point &point) const {
        /**
        mfem::Vector result(2);
        mfem::IntegrationPoint integrationPoint{};
        integrationPoint.Set2(point.x, point.y);

        //auto transformation = this->h1Space.GetElementTransformation(element.getId());
        //transformation->SetIntPoint(&integrationPoint);
        this->_density.GetVectorValue(element.getId(), integrationPoint, result);

        return {result[0], result[1]};
        **/
        mfem::Vector result(2);
        mfem::IntegrationPoint integrationPoint{};
        integrationPoint.Set2(point.x, point.y);

        auto transformation = this->h1Space.GetElementTransformation(element.getId());
        transformation->SetIntPoint(&integrationPoint);
        this->_density.GetGradient(*transformation, result);

        return {result[0], result[1]};
    }

    mfem::GridFunction H1Gradient::projectL2toH1(mfem::GridFunction &function) {
        /**
        mfem::GridFunction continuousInterpolant(&h1Space);
        mfem::DiscreteLinearOperator discreteLinearOperator(&l2Space, &h1Space);
        discreteLinearOperator.AddDomainInterpolator(new mfem::IdentityInterpolator());
        discreteLinearOperator.Assemble();
        discreteLinearOperator.Finalize();
        discreteLinearOperator.Mult(function, continuousInterpolant);

        return continuousInterpolant;

        **/

        mfem::GridFunctionCoefficient densityFunctionCoefficient(&function);
        mfem::GridFunction result(&h1Space);
        result = 0;
        // result.ProjectBdrCoefficient(densityFunctionCoefficient, ...);
        // pocet atributy
        //mesh.bdr_.max
        //... = array of attr

        mfem::MixedBilinearForm B(&l2Space, &h1Space);
        B.AddDomainIntegrator(new mfem::MixedScalarMassIntegrator);
        B.Assemble();
        B.Finalize();
        mfem::LinearForm b(&h1Space);
        B.Mult(function, b);

        mfem::BilinearForm A(&h1Space);
        A.AddDomainIntegrator(new mfem::MassIntegrator);
        A.Assemble();
        //A.EliminateEssentialBC(..., function, b);
        A.Finalize();





        mfem::GSSmoother smoother(A.SpMat());

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
        for (const auto &vector : vectors) {
            if (vector.getNorm() == 0) continue;
            numerator = numerator + vector.getNorm() * vector;
            denominator += vector.getNorm();
        }
        return denominator == 0 ? Vector(0, 0) : 1.0 / denominator * numerator;
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
        auto y = (A.xx * b.y - b.x * A.yx) / det;
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

    Vector Householder::get(
            const PointOnFace &pointOnFace,
            const Element &,
            const Element &) const {
        auto points = pointOnFace.face->getPoints();
        auto it0 = this->gradientAtPoints.find(points[0]);
        auto it1 = this->gradientAtPoints.find(points[1]);
        if (it0 != this->gradientAtPoints.end() && it1 != this->gradientAtPoints.end()){
            auto gradient0 = it0->second;
            auto gradient1 = it1->second;
            return linearInterpolate(*points[0], *points[1], pointOnFace.point, gradient0, gradient1);
        } else {
            throw std::logic_error("No gradient found!");
        }
    }

    std::map<Point *, Vector> Householder::getSmoothedGradientAtPoints() {
        std::map<Point *, Vector> result;
        for (auto &pair : this->gradientAtPoints) {
            auto point = pair.first;
            Vector gradient{};

            std::set<Point *> adjacentPoints;
            for (auto element : mesh.getPointAdjacentElements(point)) {
                for (auto adjacentPoint : element->getPoints()) {
                    adjacentPoints.insert(adjacentPoint);
                }
            }

            double scale = 0;
            for (auto adjacentPoint : adjacentPoints) {
                auto it = this->gradientAtPoints.find(adjacentPoint);
                if (it != this->gradientAtPoints.end()) {
                    const auto &adjacentGradient = it->second;
                    auto distance = (*point - *adjacentPoint).getNorm();
                    auto gauss = gaussian(distance);
                    gradient =  gradient + gauss * adjacentGradient;
                    scale += gauss;
                }
            }
            gradient = 1/scale * gradient;

            result.insert({point, gradient});
        }
        return result;
    }

    Vector Householder::getGradientAtPoint(const Point *point) const {
        int index = 0;
        auto elements = mesh.getPointAdjacentElements(point);
        if (elements.size() < 3){
            elements = {elements[0]};
            auto adjacent = mesh.getElementAdjacentElements(*elements[0]);
            elements.insert(elements.end(), adjacent.begin(), adjacent.end());
            if (elements.size() < 3){
                elements = {elements[1]};
                adjacent = mesh.getElementAdjacentElements(*elements[1]);
                elements.insert(elements.end(), adjacent.begin(), adjacent.end());
            }
        }
        rosetta::Matrix A(elements.size(), 3);
        rosetta::Matrix b(elements.size(), 1);
        for (const auto &element : elements) {
            auto centroid = getPolygonCentroid(*element);
            auto dx = centroid.x - point->x;
            auto dy = centroid.y - point->y;
            auto d = dx * dx + dy * dy;
            auto weight = 1 / std::pow(d, 0.125);
            A(index, 0) = weight;
            A(index, 1) = weight * dx;
            A(index, 2) = weight * dy;
            b(index, 0) = weight * density.getValue(*element);
            ++index;
        }
        return solveOverdetermined(A, b);
    }

    Vector Householder::linearInterpolate(const Point &a, const Point &b, const Point &x, const Vector &valueA,
                                          const Vector &valueB) {
        auto norm = (b - a).getNorm();
        auto xDistFromA = (x - a).getNorm();
        return valueA + (xDistFromA) / (norm) * (valueB - valueA);
    }

    Vector Householder::solveOverdetermined(rosetta::Matrix &A, rosetta::Matrix &b) {
        rosetta::Matrix Q, R;
        householder(A, R, Q);
        Q.trim_columns(3);
        R.trim_rows(3);

        Q.transpose();
        rosetta::Matrix Qtb;
        Qtb.mult(Q, b);
        rosetta::Matrix x(3, 1);
        x.forward_substitute(R, Qtb);
        return {x(1, 0), x(2, 0)};
    }

    std::map<Point *, Vector> Householder::getGradientAtPoints() const {
        std::map<Point *, Vector> result;
        for (const auto &point : mesh.getPoints()) {
            result.insert({point, getGradientAtPoint(point)});
        }
        return result;
    }

    void Householder::update(bool smoothing) {
        this->gradientAtPoints = this->getGradientAtPoints();
        if (smoothing){
            this->gradientAtPoints = this->getSmoothedGradientAtPoints();
        }
    }
}

