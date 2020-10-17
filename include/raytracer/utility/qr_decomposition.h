#ifndef RAYTRACER_QRDECOMPOSITION_H
#define RAYTRACER_QRDECOMPOSITION_H

#include <cstdio>
#include <cstdlib>
#include <cstring> // for memset
#include <limits>
#include <iostream>
#include <vector>

#include <math.h>

namespace rosetta {
    class Vector;

    class Matrix {

    public:
        // default constructor (don't allocate)
        Matrix();

        // constructor with memory allocation, initialized to zero
        Matrix(int m_, int n_);

        // copy constructor
        Matrix(const Matrix &mat);

        // constructor from array
        template<int rows, int cols>
        explicit Matrix(double (&a)[rows][cols]) : Matrix(rows, cols) {

            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++)
                    (*this)(i, j) = a[i][j];
        }

        // destructor
        ~Matrix();


        // access data operators
        double &operator()(int i, int j);

        double operator()(int i, int j) const;

        // operator assignment
        Matrix &operator=(const Matrix &source);

        // compute minor
        void compute_minor(const Matrix &mat, int d);

        // Matrix multiplication
        // c = a * b
        // c will be re-allocated here
        void mult(const Matrix &a, const Matrix &b);

        void forward_substitute(const Matrix &A, const Matrix &b);

        void transpose();

        void trim_columns(int columns_after);

        void trim_rows(int rows_after);


        // take c-th column of m, put in v
        void extract_column(Vector &v, int c);

        // memory allocation
        void allocate(int m_, int n_); // allocate

        // memory free
        void deallocate();

        int m, n;

    private:
        double *data;

    }; // struct Matrix

// column vector
    class Vector {

    public:
        // default constructor (don't allocate)
        Vector();

        // constructor with memory allocation, initialized to zero
        explicit Vector(int size_);

        // destructor
        ~Vector();

        // access data operators
        double &operator()(int i);

        double operator()(int i) const;

        // operator assignment
        Vector &operator=(const Vector &source);

        // memory allocation
        void allocate(int size_); // allocate

        // memory free
        void deallocate();

        //   ||x||
        double norm();

        // divide data by factor
        void rescale(double factor);

        void rescale_unit();

        int size;

    private:
        double *data;

    };


    // class Vector

// c = a + b * s
    void vmadd(const Vector &a, const Vector &b, double s, Vector &c);

// mat = I - 2*v*v^T
// !!! m is allocated here !!!
    void compute_householder_factor(Matrix &mat, const Vector &v);

    void matrix_show(const Matrix &m, const std::string &str = "");

// L2-norm ||A-B||^2
    double matrix_compare(const Matrix &A, const Matrix &B);

    void householder(Matrix &mat,
                     Matrix &R,
                     Matrix &Q);
}

#endif //RAYTRACER_QRDECOMPOSITION_H
