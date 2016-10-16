/*
* Mathematical Matrix class
* Will Spurgin
* SMU Computer Science
* 24 February 2015
*
* This Matrix class stores the data in column form.
* That is, in a n x m matrix, instead of storing the first
* row in the first m spaces of the value array, we store the
* first n values that would correspond to the first column.
* All data operations act on that assumption.
*/

#ifndef SMUCSE_MAT_H__
#define SMUCSE_MAT_H__ 

#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <iostream>

#include "vec.h"

#define INDX(i , j, rows) (j)*(rows) + (i)

typedef long unsigned int luint;

class Mat
{
protected:
    // actual matrix data
    double* data;

    // Array of vectors to operate on the data.
    Vec* vectors;

    // Flag to specify whether this instance allocated the data.
    bool owns_data = false;

    // The number of rows in the matrix.
    luint rows;

    // The number of columns in the matrix.
    luint columns;

    luint _size;

    // private copy function
    Mat& _copy(const Mat& rhs);

    // private index function for determining 1dim location from 2dim co-ord.
    luint _index(luint i, luint j) const;

    // private index function for determining 1dim location from 2dim co-ord
    // row based.
    luint _index_row(luint i, luint j) const;

    void _destroy();

public:

    // Default Constructor
    Mat();

    // Create a n by m matrix.
    Mat(luint n, luint m);

    // Create a n by m matrix with the provided data.
    Mat(luint n, luint m, double* data);

    // Copy constructor (deep copy).
    Mat(const Mat& rhs);

    // Destructor
    ~Mat();

    // Helper, Mutator, and Accessor functions //

    // Write the contents of the matrix to stdout.
    void Write() const;

    // Write the contents of the matrix to the provide ostream.
    void Write(std::ostream& out, const char delimiter = ' ') const;

    // Write the contents of the matrix to a file with name = input filename.
    void Write(const char* filename, const char delimiter = ' ') const;

    // Return the number of rows in this matrix.
    luint Rows() const;

    // Return the number of columns in this matrix.
    luint Columns() const;

    // Return pointer to the data for this matrix.
    double* GetData() const;

    // Return the pointer to the vectors of this matrix
    Vec* GetColumns() const;

    // Set data for this matrix
    void SetData(double* data, luint rows, luint cols);

    // Return reference to data value at i,j
    double& operator()(luint i, luint j) const;

    double& operator[](luint i) const { return this->data[i]; }

    /* Mathematical Function (in-place)
     * Code here based on Matrix class by Daniel R. Reynolds
     * Does not include every mathematical function
     */
    int LinearSum(double a, double b, const Mat &B);                // C = a*C + b*B
    int LinearSum(double a, const Mat &A, double b, const Mat &B);  // C = a*A + b*B
    int Add(const Mat &A) { return LinearSum(1.0, 1.0, A); };       // C = C+A
    int Add(double a);                                              // C = C+a
    int Sub(const Mat &A) { return LinearSum(1.0, -1.0, A); };      // C = C-A
    int Sub(double a) { return Add(-a); };                          // C = C-a
    int Mul(const Mat &A);                                          // C = C.*A
    int Mul(double a);                                              // C = a*C
    int Const(double a);                                            // C = a
    int Trans();                                                    // C = C^T
    int Random();                                                   // Cij is random
    int RandomDisk();                                               // C is Uniform random disk

    // in-place shortcut operators
    int operator+=(const Mat &A) { return Add(A); };  // C += A
    int operator+=(double a) { return Add(a); };      // C += a
    int operator-=(const Mat &A) { return Sub(A); };  // C -= A
    int operator-=(double a) { return Sub(a); };      // C -= a
    int operator*=(const Mat &A) { return Mul(A); };  // C *= A
    int operator*=(double a) { return Mul(a); };      // C *= a
    int operator=(double a) { return Const(a); };     // C = a

    // new matrix creation operations (C is the output, A and B are the operands)
    virtual Mat Multiply(const Mat &B);
    virtual Vec Multiply(const Vec &b);

    Mat& operator=(const Mat& B);
    Mat operator+(const Mat &B);                     // C = A+B
    Mat operator-(const Mat &B);                     // C = A-B
    Mat operator*(const Mat &B) { return this->Multiply(B); };                     // C = A*B
    Mat operator*(const double a);                   // C = B*a
    Vec operator*(const Vec &b) { return this->Multiply(b); };
    Mat T() const;                                          // C = A^T
    Vec& AccessColumn(luint j);                        // C = A(:,j)

    // Scalar output operators on matrices
    double Norm();      // sqrt(sum_ij Cij^2) (sqrt matrix Frobenius norm, vector 2-norm)
    double MaxNorm();   // max_ij |Cij|       (vector inf-norm)
    double InfNorm();   // max_i sum_j |Cij|  (row vector 1-norm, col vector inf-norm)
    double OneNorm();   // max_j sum_i |Cij|  (row vector inf-norm, col vector 1-norm)
    bool operator==(const Mat &A);

    // other scalar Matrix statistics
    double Min();       // min_ij Cij
    double Max();       // max_ij Cij 

    friend Mat operator*(const double a, const Mat& B);
    friend int GramSchmidt(Mat& A);
};

int GramSchmidt(Mat& A);

#endif
