#include "qr_decomposition.h"

namespace rosetta {
    Matrix::Matrix() : m(0), n(0), data(nullptr) {}

    // take c-th column of a matrix, put results in Vector v
    void Matrix::extract_column(Vector &v, int c) {
        if (m != v.size) {
            std::cerr << "[Matrix::extract_column]: Matrix and Vector sizes don't match\n";
            return;
        }

        for (int i = 0; i < m; i++)
            v(i) = (*this)(i, c);
    }

    Matrix::Matrix(int m_, int n_) : Matrix() {
        m = m_;
        n = n_;
        allocate(m_, n_);
    }

    Matrix::Matrix(const Matrix &mat) : Matrix(mat.m, mat.n) {

        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                (*this)(i, j) = mat(i, j);
    }

    Matrix::~Matrix() {
        deallocate();
    }

    double &Matrix::operator()(int i, int j) {
        return data[i + m * j];
    }

    double Matrix::operator()(int i, int j) const {
        return data[i + m * j];
    }

    Matrix &Matrix::operator=(const Matrix &source) {

        // self-assignment check
        if (this != &source) {
            if ((m * n) != (source.m * source.n)) { // storage cannot be reused
                allocate(source.m, source.n);          // re-allocate storage
            }
            // storage can be used, copy data
            std::copy(source.data, source.data + source.m * source.n, data);
        }
        return *this;
    }

    void Matrix::compute_minor(const Matrix &mat, int d) {

        allocate(mat.m, mat.n);

        for (int i = 0; i < d; i++)
            (*this)(i, i) = 1.0;
        for (int i = d; i < mat.m; i++)
            for (int j = d; j < mat.n; j++)
                (*this)(i, j) = mat(i, j);

    }

    void Matrix::mult(const Matrix &a, const Matrix &b) {

        if (a.n != b.m) {
            std::cerr << "Matrix multiplication not possible, sizes don't match !\n";
            return;
        }

        // reallocate ourself if necessary i.e. current Matrix has not valid sizes
        if (a.m != m or b.n != n)
            allocate(a.m, b.n);

        memset(data, 0, m * n * sizeof(double));

        for (int i = 0; i < a.m; i++)
            for (int j = 0; j < b.n; j++)
                for (int k = 0; k < a.n; k++)
                    (*this)(i, j) += a(i, k) * b(k, j);

    }

    void Matrix::forward_substitute(const Matrix &A, const Matrix &b) {
        if (A.m != A.n || A.m != b.m || b.n != 1) throw std::logic_error("Invalid matrix sizes");
        if (this->m != b.m || this->n != b.n) throw std::logic_error("Result must have same size as b");

        if (A(m-1, m-1) == 0){
            throw std::logic_error("Not a triangular matrix!");
        }

        (*this)(m - 1, 0) = b(m - 1, 0) / A(m - 1, m - 1);
        for (int k = m - 2; k >= 0; k--){
            double result = b(k, 0);
            for (int i = m - 1; i > k; i--){
                result -= A(k, i) * (*this)(i, 0);
            }
            (*this)(k, 0) = result / A(k, k);
        }
    }

    void Matrix::transpose() {
        std::vector<double> temp(data, data + m * n);
        int old_m = m;
        int old_n = n;
        allocate(n, m);
        for (int i = 0; i < old_m; i++) {
            for (int j = 0; j < old_n; j++) {
                (*this)(j, i) = temp[i + old_m * j];
            }
        }
    }

    void Matrix::trim_columns(int columns_after) {
        std::vector<double> temp(data, data + m * n);
        allocate(m, columns_after);
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                (*this)(i, j) = temp[i + m * j];
            }
        }
    }

    void Matrix::trim_rows(int rows_after) {
        std::vector<double> temp(data, data + m * n);
        int old_m = m;
        allocate(rows_after, n);
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                (*this)(i, j) = temp[i + old_m * j];
            }
        }
    }

    void Matrix::allocate(int m_, int n_) {

        // if already allocated, memory is freed
        deallocate();

        // new sizes
        m = m_;
        n = n_;

        data = new double[m_ * n_];
        memset(data, 0, m_ * n_ * sizeof(double));

    }

    void Matrix::deallocate() {

        if (data)
            delete[] data;

        data = nullptr;

    }

    Vector::Vector() : size(0), data(nullptr) {}

    Vector::Vector(int size_) : Vector() {
        size = size_;
        allocate(size_);
    }

    Vector::~Vector() {
        deallocate();
    }

    double &Vector::operator()(int i) {
        return data[i];
    }

    double Vector::operator()(int i) const {
        return data[i];
    }

    Vector &Vector::operator=(const Vector &source) {

        // self-assignment check
        if (this != &source) {
            if (size != (source.size)) {   // storage cannot be reused
                allocate(source.size);         // re-allocate storage
            }
            // storage can be used, copy data
            std::copy(source.data, source.data + source.size, data);
        }
        return *this;
    }

    void Vector::allocate(int size_) {

        deallocate();

        // new sizes
        size = size_;

        data = new double[size_];
        memset(data, 0, size_ * sizeof(double));

    }

    void Vector::deallocate() {

        if (data)
            delete[] data;

        data = nullptr;

    }

    double Vector::norm() {
        double sum = 0;
        for (int i = 0; i < size; i++) sum += (*this)(i) * (*this)(i);
        return sqrt(sum);
    }

    void Vector::rescale(double factor) {
        for (int i = 0; i < size; i++) (*this)(i) /= factor;
    }

    void Vector::rescale_unit() {
        double factor = norm();
        rescale(factor);
    }

    void vmadd(const Vector &a, const Vector &b, double s, Vector &c) {
        if (c.size != a.size or c.size != b.size) {
            std::cerr << "[vmadd]: vector sizes don't match\n";
            return;
        }

        for (int i = 0; i < c.size; i++)
            c(i) = a(i) + s * b(i);
    }

    void compute_householder_factor(Matrix &mat, const Vector &v) {

        int n = v.size;
        mat.allocate(n, n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                mat(i, j) = -2 * v(i) * v(j);
        for (int i = 0; i < n; i++)
            mat(i, i) += 1;
    }

    void matrix_show(const Matrix &m, const std::string &str) {
        std::cout << str << "\n";
        for (int i = 0; i < m.m; i++) {
            for (int j = 0; j < m.n; j++) {
                printf(" %8.3f", m(i, j));
            }
            printf("\n");
        }
        printf("\n");
    }

    double matrix_compare(const Matrix &A, const Matrix &B) {
        // matrices must have same size
        if (A.m != B.m or A.n != B.n)
            return std::numeric_limits<double>::max();

        double res = 0;
        for (int i = 0; i < A.m; i++) {
            for (int j = 0; j < A.n; j++) {
                res += (A(i, j) - B(i, j)) * (A(i, j) - B(i, j));
            }
        }

        res /= A.m * A.n;
        return res;
    }

    void householder(Matrix &mat, Matrix &R, Matrix &Q) {

        int m = mat.m;
        int n = mat.n;

        // array of factor Q1, Q2, ... Qm
        std::vector<Matrix> qv(m);

        // temp array
        Matrix z(mat);
        Matrix z1;

        for (int k = 0; k < n && k < m - 1; k++) {

            Vector e(m), x(m);
            double a;

            // compute minor
            z1.compute_minor(z, k);

            // extract k-th column into x
            z1.extract_column(x, k);

            a = x.norm();
            if (mat(k, k) > 0) a = -a;

            for (int i = 0; i < e.size; i++)
                e(i) = (i == k) ? 1 : 0;

            // e = x + a*e
            vmadd(x, e, a, e);

            // e = e / ||e||
            e.rescale_unit();

            // qv[k] = I - 2 *e*e^T
            compute_householder_factor(qv[k], e);

            // z = qv[k] * z1
            z.mult(qv[k], z1);

        }

        Q = qv[0];

        // after this loop, we will obtain Q (up to a transpose operation)
        for (int i = 1; i < n && i < m - 1; i++) {

            z1.mult(qv[i], Q);
            Q = z1;

        }

        R.mult(Q, mat);
        Q.transpose();
    }
}
