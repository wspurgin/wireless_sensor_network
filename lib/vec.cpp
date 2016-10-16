/* Daniel R. Reynolds & Will Spurgin
   SMU Mathematics & Computer Science
   Math 6370/4370
   9 March 2013 & 12 February 2015 */

// Inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <random>
#include "vec.h"


// This file implements the operations defined in the Vec class


///// General class routines /////
Vec::Vec() {
  length = 0;
  data = nullptr;
  own_data = false;
}

// constructor (initializes values to 0.0)
Vec::Vec(long int len) {
  length = len;
  data = new double[length]();
  own_data = true;
}


// constructor (overloaded, sets data to pre-allocated space)
Vec::Vec(long int len, double *user_data) {
  length = len;
  data = user_data;
  own_data = false;
}

// copy constructor (deep copy)
Vec::Vec(const Vec& rhs) {
  _copy(rhs);
}

// destructor (frees space associated with a Vec)
Vec::~Vec() {
  if (own_data)  delete[] data;
  own_data = false;
  length = 0;
}

Vec& Vec::_copy(const Vec& rhs) {
  length = rhs.length;
  data = new double[length]();
  for (int i = 0; i < length; ++i) {
    data[i] = rhs.data[i];
  }
  own_data = true;
  return *this;
}


// clone routine (clone myself to create a new Vec)
Vec* Vec::Clone() const {
  return new Vec(length);
};


// write myself to stdout
int Vec::Write() const {
  // return with failure if data array isn't allocated
  if (data == nullptr) {
    fprintf(stderr, "Vec:Write error, empty data array\n");
    return 1;
  }

  // print data to screen
  for (long int i=0; i<length; i++)
    printf("  %g\n",data[i]);
  printf("\n");

  // return with success
  return 0;
}


// write myself to a file
int Vec::Write(const char *outfile) const {
  // return with failure if data array isn't allocated
  if (data == nullptr) {
    fprintf(stderr, "Vec:Write error, empty data array\n");
    return 1;
  }

  // return with failure if 'outfile' is empty
  if (strlen(outfile) < 1) {
    fprintf(stderr, "Vec::Write error, empty outfile\n");
    return 1;
  }

  // open output file
  FILE *fptr = nullptr;
  fptr = fopen(outfile, "w");
  if (fptr == nullptr) {
    fprintf(stderr, "Vec::Write error, unable to open %s for writing\n",outfile);
    return 1;
  }

  // print data to file
  for (long int i=0; i<length; ++i)
    fprintf(fptr, "  %g\n",data[i]);
  fprintf(fptr, "\n");

  // close output file
  fclose(fptr);

  // return with success
  return 0;
}


// returns my length
long int Vec::Length() const {
  return length;
};


// set my data array
int Vec::SetData(double *DataArray, long int length) {
  // delete data array if I own it
  if (own_data) {
    delete[] data;
    own_data = false;
  }

  // set data pointer to new data array
  data = DataArray;
  this->length = length;

  // return with success
  return 0;
}


// get my data array
double* Vec::GetData() const {
  return data;
};

Vec& Vec::operator=(const Vec& rhs) {
  if(own_data && data != nullptr && length > 0) {
    delete[] data;
    length = 0;
    own_data = false;
  }
  _copy(rhs);
  return *this;
}

Vec Vec::operator-(const Vec& y) const {
  Vec diff(*this);
  diff -= y;
  return diff;
}

Vec Vec::operator+(const Vec& y) const {
  Vec sum(*this);
  sum += y;
  return sum;
}

// access my data -- access value by location in array,
// gets reference to value so that it can be changed (1-based version)
double& Vec::operator()(long int i) {
  // check that data is not nullptr
  if (data == nullptr) {
    fprintf(stderr, "Vec() error: empty data array\n");
    return data[0];
  }

  // check that i is within data length; if not return 0.0
  if (i<0 || i>=length) {
    fprintf(stderr,"Vec() error: %li outside vector bounds\n",i);
    fprintf(stderr,"  returning first entry to avoid seg. fault\n");
    return data[0];
  }
  return data[i];
}



// access my data -- access value by location in array,
// gets reference to value so that it can be changed (0-based version)
double& Vec::operator[](long int i) {
  // check that data is not nullptr
  if (data == nullptr) {
    fprintf(stderr, "Vec[] error: empty data array\n");
    return data[0];
  }

  // check that i is within data length; if not return 0.0
  if (i<0 || i>=length) {
    fprintf(stderr,"Vec[] error: %li outside vector bounds\n",i);
    fprintf(stderr,"  returning first entry to avoid seg. fault\n");
    return data[0];
  }
  return data[i];
}



///// Arithmetic operations defined on a given Vec /////

// x = a*x + b*y
int Vec::LinearSum(double a, double b, const Vec &y) {
  // check that array sizes match
  if (y.length != length) {
    fprintf(stderr,"Vec::LinearSum error, vector sizes do not match\n");
    return 1;
  }

  // check that data is not nullptr
  if (data == nullptr || y.data == nullptr) {
    fprintf(stderr, "Vec::LinearSum error: empty data array\n");
    return 1;
  }

  // perform operation
  for (long int i=0; i<length; i++)
    data[i] = a*data[i] + b*y.data[i];

  // return success
  return 0;
}


// x = a*y + b*z
int Vec::LinearCombination(double a, const Vec &y, double b, const Vec &z) {
  // check that array sizes match
  if (y.length != length  ||  z.length != length) {
    fprintf(stderr,"Vec::LinearSum error, vector sizes do not match\n");
    return 1;
  }

  // check that data is not nullptr
  if (data == nullptr || y.data == nullptr || z.data == nullptr) {
    fprintf(stderr, "Vec::LinearSum error: empty data array\n");
    return 1;
  }

  // perform operation
  for (long int i=0; i<length; i++)
    data[i] = a*y.data[i] + b*z.data[i];

  // return success
  return 0;
}


// x = x+y  (adds vector y to my data)
int Vec::Add(const Vec& y) {
  return LinearSum(1.0, 1.0, y);
};


//   x = x+a  (adds scalar a to my data)
int Vec::Add(double a) {
  // check that data is not nullptr
  if (data == nullptr) {
    fprintf(stderr, "Vec::Add error: empty data array\n");
    return 1;
  }

  // perform operation
  for (long int i=0; i<length; i++)  data[i] += a;

  // return success
  return 0;
}


// x = x-y  (subtracts vector y from my data)
int Vec::Sub(const Vec& y) {
  return LinearSum(1.0, -1.0, y);
};


// x = x-a  (subtracts scalar a from my data)
int Vec::Sub(double a) {
  return Add(-a);
};


//   x = x.*y (multiplies my data by y, component-wise)
int Vec::Mul(const Vec &y) {
  // check that array sizes match
  if (y.length != length) {
    fprintf(stderr,"Vec::Mul error, vector sizes do not match\n");
    return 1;
  }

  // check that data is not nullptr
  if (data == nullptr || y.data == nullptr) {
    fprintf(stderr, "Vec::Mul error: empty data array\n");
    return 1;
  }

  // perform operation
  for (long int i=0; i<length; i++)  data[i] *= y.data[i];

  // return success
  return 0;
}


//   x = x*a  (scales my data by scalar a)
int Vec::Scale(double a) {
  // check that data is not nullptr
  if (data == nullptr) {
    fprintf(stderr, "Vec::Scale error: empty data array\n");
    return 1;
  }

  // perform operation
  for (long int i=0; i<length; i++)  data[i] *= a;

  // return success
  return 0;
}


//   x = x./y (divides my data by y, component-wise)
int Vec::Div(const Vec &y) {
  // declarations
  long int i;
  bool OK;

  // check that array sizes match
  if (y.length != length) {
    fprintf(stderr,"Vec::Div error, vector sizes do not match\n");
    return 1;
  }

  // check that data is not nullptr
  if (data == nullptr || y.data == nullptr) {
    fprintf(stderr, "Vec::Div error: empty data array\n");
    return 1;
  }

  // check that y is nowhere zero
  OK=true;
  for (i=0; i<length; i++)
    if (y.data[i] == 0.0) {
      OK = false;
      break;
    }
  if (!OK) {
    fprintf(stderr,"Vec::Div error, divisor contains 0.0 entry\n");
    return 1;
  }

  // perform operation
  for (i=0; i<length; i++)  data[i] /= y.data[i];

  // return success
  return 0;
}


//   x = y  (copies y into x)
int Vec::Copy(const Vec &y) {
  // check that array sizes match
  if (y.length != length) {
    fprintf(stderr,"Vec::Copy error, vector sizes do not match\n");
    return 1;
  }

  // check that data is not nullptr
  if (data == nullptr || y.data == nullptr) {
    fprintf(stderr, "Vec::Copy error: empty data array\n");
    return 1;
  }

  // perform operation
  for (long int i=0; i<length; i++)  data[i] = y.data[i];

  // return success
  return 0;
}


//   x = a  (sets all entries of x to the scalar a)
int Vec::Const(double a) {
  // check that data is not nullptr
  if (data == nullptr) {
    fprintf(stderr, "Vec::Const error: empty data array\n");
    return 1;
  }

  // perform operation
  for (long int i=0; i<length; i++)  data[i] = a;

  // return success
  return 0;
}


// x = x.^p
int Vec::Power(double p) {
  // check that data is not nullptr
  if (data == nullptr) {
    fprintf(stderr, "Vec::Power error: empty data array\n");
    return 1;
  }

  // perform operation
  for (long int i=0; i<length; i++)  data[i] = pow(data[i], p);

  // return success
  return 0;
}


//   x == y  (checks whether vectors are equal)
bool Vec::Equals(const Vec &y) {
  if (y.length != length)
    return false;
  if (data == nullptr || y.data == nullptr)
    return false;
  for (long int i=0; i<length; i++)
    if (data[i] != y.data[i])
      return false;
  return true;
}

//   Modify the values in the vector so they are the absolute values.
int Vec::Abs() {
  if (data == nullptr) {
    fprintf(stderr, "Vec::Abs error: empty data array\n");
    return 1;
  }

  for (long int i = 0; i < length; ++i) data[i] = fabs(data[i]);
  return 0;
}

//  Modify the vector to give it unit magnitudes
int Vec::Normalize() {
  double twoNorm = this->TwoNorm();
  if (twoNorm < 1e-13) {
    fprintf(stderr, "Vec::Normalize error: zero vector normalization\n");
    return 1;
  }
  for (long int i = 0; i < length; ++i) data[i] /= twoNorm;
  return 0;
}

//  Calculates and returns the l2 norm of the vector
double Vec::TwoNorm() {
  if (data == nullptr) {
    fprintf(stderr, "Vec::TwoNorm error: empty data array\n");
    return -1.0;
  }
  double twoNorm = 0.0;
  for (long int i = 0; i < length; ++i) twoNorm += data[i]*data[i];
  return sqrt(twoNorm);
}

//  Calculates and returns the root-mean-square norm of the vector
double Vec::RmsNorm() {
  if (data == nullptr) {
    fprintf(stderr, "Vec::RmsNorm error: empty data array\n");
    return -1.0;
  }
  double twoNorm = 0.0;
  for (long int i = 0; i < length; ++i) twoNorm += data[i]*data[i];
  return sqrt(twoNorm/length);
}

// Calculates and returns the infinity norm of the vector
double Vec::MaxNorm() {
  if (data == nullptr) {
    fprintf(stderr, "Vec::MaxNorm error: empty data array\n");
    return -1.0;
  }
  double max = 0.0;
  for (long int i = 0; i < length; ++i) {
    auto val = fabs(data[i]);
    if (val > max)
      max = val;
  }
  return max;
}

//  Finds the smallest entry in the vector
double Vec::Min() {
  auto index = this->MinLoc();
  if (index == -1) {
    fprintf(stderr, "%s error: empty data array\n", "Vec::Min");
    return index;
  }
  return data[index];
}

//  Finds the largest entry in the vector
double Vec::Max() {
  auto index = this->MaxLoc();
  if (index == -1) {
    fprintf(stderr, "%s error: empty data array\n", "Vec::Max");
    return index;
  }
  return data[index];
}

//  Finds the index of the smallest entry in the vector
long int Vec::MinLoc() {
  if (data == nullptr) {
    fprintf(stderr, "%s error: empty data array\n", "Vec::MinLoc");
    return -1L;
  }
  long int index = 0;
  double min = data[index];
  for (long int i = 1; i < length; ++i) {
    double val = data[i];
    if (val < min) {
      min = val;
      index = i;
    }
  }
  return index;
}

//  Finds the index of the largest entry in the vector
long int Vec::MaxLoc() {
  if (data == nullptr) {
    fprintf(stderr, "%s error: empty data array\n", "Vec::MaxLoc");
    return -1L;
  }
  long int index = 0;
  double max = data[index];
  for (long int i = 1; i < length; ++i) {
    double val = data[i];
    if (val > max) {
      max = val;
      index = i;
    }
  }
  return index;
}

//// Statistical Opeations ////

double Vec::Mean() {
  if (this->length == 0) return 0.0;
  double mean = 0.0;
  for (long int i = 0; i < this->length; ++i)
    mean += this->data[i];
  return mean / length;
}

double Vec::Std() {
  if (this->length == 0) return 0.0;
  double mean = this->Mean();
  double variance = 0.0;
  for (long int i = 0; i < this->length; ++i)
    variance += pow(this->data[i] - mean, 2);
  variance /= this->length;
  return sqrt(variance);
}

//// Helper functions ////

//  Create a vector of linearly spaced values between [a,b] (inclusively)
Vec Linspace(double a, double b, long int n) {
  if (n < 1) {
    Vec x(0);
    return x;
  }
  Vec x(n);
  double inc = (b - a) / (n - 1);

  // initialize first & last value
  x.data[0] = a;
  x.data[n-1] = b;

  // initialize remaining values
  for (long int i = 1; i < n - 1; ++i)
    x.data[i] = x.data[i - 1] + inc;

  return x;
}

//  Create a vector of logarithmically spaced values [10^a, 10^b] (inclusively)
Vec Logspace(double a, double b, long int n) {
  if (n < 1) {
    Vec y(0);
    return y;
  }
  auto min = log10(a);
  auto max = log10(b);
  auto inc = (max - min) / (n - 1);

  Vec y(n);

  // initialize first & last value
  y.data[0] = min;
  y.data[n-1] = max;

  // initialize remaining eyponents
  for (long int i = 1; i < n - 1; ++i)
    y.data[i] = y.data[i - 1] + inc;

  // initialize actual values
  for (long int i = 0; i < n; ++i){
    y.data[i] = pow(10, y.data[i]);
  }

  return y;
}

// Returns a vector of uniformally distributed values between 0 and 1.
Vec Random(long int n) {
  if (n < 1) {
    Vec z(0);
    return z;
  }
  Vec z(n);
  double divisor = pow(2, 31) - 1.0;
  for (long int i = 0; i < n; ++i) {
    double val = (double) random() / divisor;
    z.data[i] = val;
  }
  return z;
}

// Computes the dot product of the two vectors x and y.
double Dot(const Vec& x, const Vec& y) {
  if (x.data == nullptr || y.data == nullptr || x.length != y.length)
    return 0.0;
  double dotProduct = 0.0;
  for (long int i = 0; i < x.length; ++i) dotProduct += x.data[i] * y.data[i];
  return dotProduct;
}

// end of file
