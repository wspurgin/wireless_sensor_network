/* Daniel R. Reynolds & Will Spurgin
   SMU Mathematics & Computer Science
   Math 6370/4370
   9 March 2013 & 12 February 2015 */

#ifndef VEC_DEFINED__
#define VEC_DEFINED__

// Inclusions
#include <math.h>


// This defines a simple arithmetic vector class
class Vec {

 private:

  ///// Contents /////
  long int length;
  double* data;
  bool own_data;

  Vec& _copy(const Vec& rhs);

 public:

  ///// General class routines /////
  // default constructor
  Vec();

  // constructor (initializes values to 0.0)
  Vec(long int len);

  // constructor (overloaded, sets data to pre-allocated space)
  Vec(long int len, double* user_data);

  // copy constructor (deep copy)
  Vec(const Vec& rhs);

  // destructor
  ~Vec();

  // clone myself to create a new Vec
  Vec* Clone() const;

  // write myself to stdout
  int Write() const;

  // write myself to a file
  int Write(const char* outfile) const;

  // returns my length
  long int Length() const;

  // set/get my data array
  int SetData(double* DataArray, long int length);
  double* GetData() const;

  // access my data -- access value by location in array, 
  // returns handle to value so that it can be changed as well
  double& operator()(long int i);  // 1-based
  double& operator[](long int i);  // 0-based


  ///// Arithmetic operations defined on a Vec /////

  // component-wise operators
  int LinearSum(double a, double b, const Vec& y);                // x = a*x + b*y
  int LinearCombination(double a, const Vec& y, double b, const Vec& z);  // x = a*y + b*z
  int Add(const Vec& y);                                          // x = x+y
  int Add(double a);                                              // x = x+a
  int Sub(const Vec& y);                                          // x = x-y
  int Sub(double a);                                              // x = x-a
  int Mul(const Vec& y);                                          // x = x.*y
  int Scale(double a);                                            // x = x*a
  int Div(const Vec& y);                                          // x = x./y
  int Copy(const Vec& y);                                         // x = y
  int Const(double a);                                            // x = a
  int Power(double p);                                            // x = x.^p
  bool Equals(const Vec& y);                                      // x == y
  
  // in place modification operations
  int Abs();
  int Normalize();

  // Norm operations
  double TwoNorm();
  double RmsNorm();
  double MaxNorm();

  // finding min & max value
  double Min();
  double Max();
  long int MinLoc();
  long int MaxLoc();

  // statistical operations //

  // Find the mean value of data
  double Mean();

  // Compute the standard deviation of data
  double Std();

  // Shortcut operators
  int operator+=(const Vec& y) { return Add(y); };     // x += y
  int operator+=(double a) { return Add(a); };         // x += a
  int operator-=(const Vec& y) { return Sub(y); };     // x -= y
  int operator-=(double a) { return Sub(a); };         // x -= a
  int operator*=(const Vec& y) { return Mul(y); };     // x *= y
  int operator*=(double a) { return Scale(a); };       // x *= a
  int operator/=(const Vec& y) { return Div(y); };     // x /= y
  int operator/=(double a) { return Scale(1.0/a); };   // x /= a
  int operator^=(int p) { return Power(p); };          // x = x.^p
  int operator^=(double p) { return Power(p); };       // x = x.^p
  Vec& operator=(const Vec& y);
  int operator=(double a) { return Const(a); };        // x = a
  bool operator==(const Vec& y) {return Equals(y); };  // x == y

  //declaring friend Helper functions //
  friend Vec Linspace(double a, double b, long int n);
  friend Vec Logspace(double a, double b, long int n);
  friend Vec Random(long int n);
  friend double Dot(const Vec& x, const Vec& y);

};  // end Vec

// Actual declaration of helper functions
Vec Linspace(double a, double b, long int n);
Vec Logspace(double a, double b, long int n);
Vec Random(long int n);
double Dot(const Vec& x, const Vec& y);

#endif
