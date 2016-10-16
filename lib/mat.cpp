/*
* Will Spurgin
* SMU Computer Science
* 25 February 2015
*
* Matrix class Implementation
* Adapted from Daniel R. Reynolds' implementation
*/

#include <fstream>
#include <stdexcept>
#include <random>

#include "mat.h"

#ifdef _OPENMP
  #include "omp.h"
#endif

/*
* Private function that acts as a deep copy constructor.
* Inputs:
*   rhs - A constant references to a Mat object that will be deep copied.
*/
Mat& Mat::_copy(const Mat& rhs) {
    this->rows = rhs.rows;
    this->columns = rhs.columns;
    this->_size = rhs._size;

    if (_size) {
        data = new double[_size]();
        owns_data = true;
        for (luint i = 0; i < _size; ++i)
            this->data[i] = rhs.data[i];

        // Set up vectors
        vectors = new Vec[columns];
        for (luint i = 0; i < columns; ++i)
            vectors[i].SetData(data+(i*rows), rows);
    }
    return *this;
}

/*
* Return one dimensional index location from mutlidemensional index coordinates
*  Inputs:
*   i - long unsiged int coressponding to row index
*   j - long unsigned int coressponding to column index
*/
luint Mat::_index(luint i, luint j) const {
    return (j)*(rows) + (i);
}

/*
* Return one dimensional index location from mutlidemensional index coordinates
* row-based.
*  Inputs:
*   i - long unsiged int coressponding to row index
*   j - long unsigned int coressponding to column index
*/
luint Mat::_index_row(luint i, luint j) const {
  return (j)*(columns) + (i);
}

/*
* Private function that will 'destroy' any dynamically allocated memory for
* this object safely.
*/
void Mat::_destroy() {
    if (owns_data && data) {
      delete [] data;
      rows = 0;
      columns = 0;
      _size = 0;
      data = nullptr;
    }
    if (vectors) {
      delete [] vectors;
      vectors = nullptr;
    }
}

/*
* Default Constructor
*/
Mat::Mat() {
    data = nullptr;
    vectors = nullptr;
    rows = columns = _size = 0;
    owns_data = false;
}

/*
* Constructor
* Creates a Matrix of n x m dimensions, initially the n x m zero matrix.
* Inputs:
*   n - long unsigned int for the number of rows.
*   m - long unsigned int for the number of columns.
*/
Mat::Mat(luint n, luint m) {
    if (n < 1) {
        fprintf(stderr, "Attempting to create Matrix with %lu rows. Reseting to 1\n", n);
        n = 1;
    }
    if (m < 1) {
        fprintf(stderr, "Attempting to create Matrix with %lu columns. Reseting to 1\n", m);
        m = 1;
    }
    rows = n;
    columns = m;
    _size = rows * columns;
    data = new double[_size]();
    owns_data = true;

    // Set vectors to operate on column-wise data.
    vectors = new Vec[columns];
    for (luint i = 0; i < columns; ++i)
        vectors[i].SetData(data+(i*rows), rows);
}

/*
* Constructor
* Creates a Matrix of n x m dimensions and set its data to the provided.
* Inputs:
*   n - The number of rows.
*   m - The number of columns.
*   data - The data for the matrix. Note it should be n*m in length to avoid
*       loss of data and invalid reads/writes.
*/
Mat::Mat(luint n, luint m, double* data) {
    if (n < 1) {
        fprintf(stderr, "Attempting to create Matrix with %lu rows. Reseting to 1\n", n);
        n = 1;
    }
    if (m < 1) {
        fprintf(stderr, "Attempting to create Matrix with %lu columns. Reseting to 1\n", m);
        m = 1;
    }
    rows = n;
    columns = m;
    _size = rows * columns;
    this->data = data;
    this->owns_data = false;

    // Set vectors to operate on column-wise data.
    vectors = new Vec[columns];
    for (luint i = 0; i < columns; ++i) {
        vectors[i].SetData(data+(i*rows), rows);
    }
}

/*
* Copy Constructor
* Inputs:
*   rhs - The Mat object to be copied.
*/
Mat::Mat(const Mat& rhs) { this->_copy(rhs); }

/*
* Destructor
*/
Mat::~Mat() {
    this->_destroy();
}

/*
* Member function that writes the contents of the matrix to stdout.
*/
void Mat::Write() const {
    Write(std::cout);
}

/*
* Member function that writes the contents of the matrix to the provided
* ostream.
* Inputs:
*   out - The ostream to which the matrix will be written.
*/
void Mat::Write(std::ostream& out, const char delimiter) const {
    out.precision(16);
    for (luint i = 0; i < rows; ++i) {
        for (luint j = 0; j < columns - 1; ++j) {
            out << std::fixed << data[_index(i, j)] << delimiter;
        }
        out << std::fixed << data[_index(i, columns - 1)] << std::endl;
    }
}

/*
* Member function that writes the contents of the matrix to a file
* specified by the provided filename.
* Inputs:
*   filename - The name of the file to which the matrix will be written.
*/
void Mat::Write(const char* filename, const char delimiter) const {
    std::ofstream fout(filename);
    if (fout)
        Write(fout, delimiter);
    else
        fprintf(stderr, "File '%s' could not be opened.\n", filename);
}

/*
* Accessor for the Row dimension of the matrix.
*/
luint Mat::Rows() const { return this->rows; }

/*
* Accessor for the Column dimension of the matrix.
*/
luint Mat::Columns() const { return this->columns; }

/*
* Accessor for the matrix data.
*/
double* Mat::GetData() const { return this->data; }

/*
* Accessor for the matrix vectors.
*/
Vec* Mat::GetColumns() const { return this->vectors; }

/*
* Mutator for the matrix data.
* Inputs:
*   data - The data that the matrix will use. Note it should be equal to
*       (input)rows * (input)columns in length to avoid data loss or invalid
*       reads/writes.
*   rows - The new number of rows.
*   columns  - the new number of columns.
*/
void Mat::SetData(double* data, luint rows, luint columns) {
    this->_destroy(); // will destroy data if we've already allocated some.
    this->rows = rows;
    this->columns = columns;
    this->_size = rows * columns;
    this->data = data;
    this->owns_data = false;

    // Set vectors to operate on column-wise data.
    vectors = new Vec[columns];
    for (luint i = 0; i < columns; ++i) {
        vectors[i].SetData(data+(i*rows), rows);
    }
}

/*
* Accessor operator for a specific data value in the Matrix.
* Inputs:
*   i - the row index of the desired data value.
*   j - the column index of the desired data value.
* Exceptions:
*   Throws runtime_error
*/
double& Mat::operator()(luint i, luint j) const {
    if (!data) {
        throw std::runtime_error("Trying to get data from empty matrix\n");
    }
    if (i > rows || j > columns) { // out of bounds indices
        char err_msg[100];
        sprintf(err_msg,
            "%lu, %lu are invalid indices for a 0-indexed (%lu, %lu) matrix\n",
            i, j, rows, columns);
        throw std::runtime_error(err_msg);
    }
    return data[_index(i, j)];
}

///// Arithmetic operations defined on a given Mat /////
/* adapted from Daniel R. Reynolds' matrix class */

// C = a*C + b*B
int Mat::LinearSum(double a, double b, const Mat &B) {
  // check that array sizes match
  if (B.rows != rows || B.columns != columns) {
    return 1;
  }

  // check that data is initialized
  if (data == nullptr || B.data == nullptr) {
    return 1;
  }

  // perform operation
  for (luint i=0; i<columns; ++i)
    vectors[i].LinearSum(a, b, B.vectors[i]);

  // return success
  return 0;
}


// C = a*A + b*B
int Mat::LinearSum(double a, const Mat &A, double b, const Mat &B) {
  // check that array sizes match
  if (A.rows != rows || A.columns != columns || B.rows != rows || B.columns != columns) {
    return 1;
  }

  // check that data is not nullptr
  if (data == nullptr|| A.data == nullptr || B.data == nullptr) {
    return 1;
  }

  // perform operation
  for (luint i=0; i<columns; ++i)
    vectors[i].LinearCombination(a, A.vectors[i], b, B.vectors[i]);

  // return success
  return 0;
}


//   C = C+a  (adds scalar a to my data)
int Mat::Add(double a) {
  // check that data is not nullptr
  if (data == nullptr) {
    return 1;
  }

  // perform operation
  for (luint i=0; i<_size; ++i)  data[i] += a;

  // return success
  return 0;
}


//   C = C.*A (multiplies my data by y, component-wise)
int Mat::Mul(const Mat &A) {
  // check that array sizes match
  if (A.rows != rows || A.columns != columns) {
    throw std::runtime_error("Mistmatch in Matrix dimensions");
  }

  // check that data is not nullptr
  if (data == nullptr || A.data == nullptr) {
    throw std::runtime_error("Unallocated Matrix data");
  }

  // perform operation
  for (luint i=0; i<_size; ++i)  data[i] *= A.data[i];

  // return success
  return 0;
}


//   C = a*C  (scales my data by scalar a)
int Mat::Mul(double a) {
  // check that data is not nullptr
  if (data == nullptr) {
    throw std::runtime_error("Unallocated Matrix data");
  }

  // perform operation
  for (luint i=0; i<_size; ++i)  data[i] *= a;

  // return success
  return 0;
}

//   C = a  (sets all entries of C to the scalar a)
int Mat::Const(double a) {
  // check that data is not nullptr
  if (data == nullptr) {
    return 1;
  }

  // perform operation
  for (luint i=0; i<_size; ++i)  data[i] = a;

  // return success
  return 0;
}

// Cij = Cji
int Mat::Trans() {
  // check that data is not nullptr
  if (data == nullptr) {
    return 1;
  }

  // perform operation in place if matrix is square
  if (rows == columns) {
    double tmp;
    for (luint i=0; i<rows; ++i)
      for (luint j=0; j<i; ++j) {
        tmp = data[_index(i,j)];
        data[_index(i,j)] = data[_index(j,i)];
        data[_index(j,i)] = tmp;
      }

  // otherwise we need a new data array to ensure a clean transpose
  } else {

    // create temporary data array, and copy transposed data over
    double *newdata = new double[_size]();
    for (luint i=0; i<rows; ++i)
      for (luint j=0; j<columns; ++j)
        newdata[_index_row(j,i)] = data[_index(i,j)];

    // copy newdata values into existing data array
    for (luint i=0; i<_size; ++i)
      data[i] = newdata[i];

    // delete temporary data array
    delete[] newdata;

    // swap matrix dimensions
    int tmp = rows;
    rows = columns;
    columns = tmp;

    // Set vectors to operate on column-wise data.
    delete [] vectors;
    vectors = new Vec[columns];
    for (luint i = 0; i < columns; ++i)
        vectors[i].SetData(data+(i*rows), rows);
  }

  // return success
  return 0;
}

// fill in a vector with uniformly-distributed random numbers in [0,1]
int Mat::Random() {
  // fill in entries and return
  double divisor = (pow(2.0,31.0) - 1.0);
  for (luint i=0; i<_size; ++i)
    data[i] = random() / divisor;
  return 0;
}

int Mat::RandomDisk() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> r(0, 1 + 1e-20);
  std::uniform_real_distribution<> theta(0, 2 * M_PI + 1e-20);
  for (luint i = 0; i < rows; i++) {
    for(luint j = 0; j < columns; j+=2) {
      double normalizer = sqrt(r(gen));
      double angle      = theta(gen);
      data[_index(i, j)] = normalizer * cos(angle);
      data[_index(i, j+1)] = normalizer * sin(angle);
    }
  }
  return 0;
}

///// New matrix creation operations /////
/* Modified version of original matrix functions by Daniel R. Reynolds */

/*
* Assignment operator (gets called on already existing instances, see copy
* constructor for construction assignment).
* Input B - Mat object to be copied
*/
Mat& Mat::operator=(const Mat &B) {
  this->_destroy();
  this->_copy(B);
  return *this;
}

// C = A+B
Mat Mat::operator+(const Mat &B) {
  // check that array sizes match
  if (B.rows != rows || B.columns != columns) {
    throw std::runtime_error("Mismatch in Matrix dimensions");
  }

  // create Mat for output, and do operation
  Mat C(*this);
  C += B;

  // return result
  return C;
}

// C = A-B
Mat Mat::operator-(const Mat &B) {
  // check that array sizes match
  if (B.rows != rows || B.columns != columns) {
    throw std::runtime_error("Mismatch in Matrix dimensions");
  }

  // create new Mat for output, and do operation
  Mat C(*this);
  C -= B;

  // return result
  return C;
}

// C = A*B
Mat Mat::Multiply(const Mat &B) {

  // determine if either matrix is a scalar
  bool A_scalar = ((rows==1) && (columns==1));
  bool B_scalar = ((B.rows==1) && (B.columns==1));

  // scalar-times-matrix
  if (A_scalar) {

    // create new Mat for output, and do operation
    Mat C(B.rows,B.columns);
    for (luint i=0; i<B._size; ++i)
      C.data[i] = data[0] * B.data[i];

  return C;
  // scalar-times-matrix
  } else if (B_scalar) {

    // create new Mat for output, and do operation
    Mat C(rows,columns);
    for (luint i=0; i<_size; ++i)
      C.data[i] = data[i] * B.data[0];

  return C;
  // normal matrix product
  } else {

    // check that array sizes are acceptable
    if (B.rows != columns) {
      throw std::runtime_error("Mismatch in Matrix dimensions");
    }

    // create new Mat for output, and do operation
    Mat C(rows,B.columns);
    luint c_rows = rows;
    luint b_rows = B.rows;
    luint b_columns = B.columns;
    #pragma omp parallel for collapse(2) firstprivate(c_rows, b_rows, b_columns)
    for (luint i=0; i<rows; ++i)
      for (luint j=0; j<B.columns; ++j)
        for (luint k=0; k<columns; ++k)
          C.data[INDX(i,j,c_rows)] += data[INDX(i,k,c_rows)] * B.data[INDX(k,j,b_rows)];
  return C;
  }
}

// C = B*a
Mat Mat::operator*(const double b) {
  // create a pointer to the result
  Mat C(rows,columns);

  for (luint i=0; i<rows*columns; i++)
    C.data[i] = data[i] * b;

  // return result
  return C;
}

/*
* Matrix vector product
* Input - b: A vector to be multiplied with the current matrix
* Output - The resultant product vector
*/
Vec Mat::Multiply(const Vec& b) {
  if (b.Length() != columns) {
    throw std::runtime_error("Mismatch in Matrix Vector product");
  }
  Vec c(rows);
  auto c_data = c.GetData();
  auto b_data = b.GetData();
  auto c_rows = rows;
  #pragma omp parallel for firstprivate(c_rows) schedule(runtime)
  for (long int i = 0; i < rows; ++i) {
    for (long int j = 0; j < columns; ++j) {
      c_data[i] += b_data[j] * this->data[INDX(i,j,c_rows)];
    }
  }
  return c;
}

// C = a*B
Mat operator*(const double a, const Mat &B) {
  // create a pointer to the result
  Mat C(B.Rows(),B.Columns());
  C.LinearSum(0.0, a, B);

  // return result
  return C;
}

// C = A^T
Mat Mat::T() const {
  // create new Mat for output, and do operation
  Mat C(columns,rows);
  for (luint j=0; j<columns; ++j)
    for (luint i=0; i<rows; ++i)
      C.data[C._index(j,i)] = data[_index(i,j)];

  // return result
  return C;
}

// column accessor routine (does not alocate its own data, only returns a
// Vec object whose data array points to an existing Mat column).
Vec& Mat::AccessColumn(luint j) {
  // check that requested column exists
  if (j < 0 || j > columns) {
    throw std::invalid_argument("Column index out of range");
  }

  // return result
    return this->vectors[j];
}

// equivalence-checking operator
bool Mat::operator==(const Mat &A)  {
  // check that array sizes match
  if (A.rows != rows || A.columns != columns) {
    return false;
  }

  // perform operation and return
  bool equal=true;
  for (luint i=0; i<_size; ++i) {
    if (A.data[i] != data[i]) return false;
  }
  return equal;
}

///// Scalar output operators on matrices /////

// vector 2-norm, square root of the matrix Frobenius norm
double Mat::Norm() {
  // check that my data is allocated
  if (data == nullptr) {
    return -1.0;
  }

  // perform operation
  double sum=0.0;
  for (luint i=0; i<_size; ++i)  sum += data[i]*data[i];
  return sqrt(sum);
}

// vector infinity norm, largest absolute value entry of a matrix
double Mat::MaxNorm() {
  // check that my data is allocated
  if (data == nullptr) {
    return -1.0;
  }

  // perform operation
  double mx=0.0;
  for (luint i=0; i<_size; ++i) {
    double a = fabs(data[i]);
    mx = (mx > a) ? mx : a;
  }
  return mx;
}

// row vector one norm, column vector infinity norm, matrix infinity norm
double Mat::InfNorm() {
  // check that my data is allocated
  if (data == nullptr) {
    return -1.0;
  }

  // perform operation
  double mx=0.0;
  for (luint i=0; i<rows; ++i) {
    double sum=0.0;
    for (luint j=0; j<columns; ++j)
      sum += fabs(data[_index(i,j)]);
    mx = (mx > sum) ? mx : sum;
  }
  return mx;
}

// row vector infinity norm, column vector one norm, matrix one norm
double Mat::OneNorm() {
  // check that my data is allocated
  if (data == nullptr) {
    return -1.0;
  }

  // perform operation
  double mx=0.0;
  for (luint j=0; j<columns; ++j) {
    double sum=0.0;
    for (luint i=0; i<rows; ++i)
      sum += fabs(data[_index(i,j)]);
    mx = (mx > sum) ? mx : sum;
  }
  return mx;
}

// min Cij
double Mat::Min() {
  // check that my data is allocated
  if (data == nullptr) {
    return -1.0;
  }

  // perform operation
  double mn=data[0];
  for (luint i=0; i<_size; ++i)
    mn = (mn < data[i]) ? mn : data[i];
  return mn;
}


// max Cij
double Mat::Max() {
  // check that my data is allocated
  if (data == nullptr) {
    return -1.0;
  }

  // perform operation
  double mx=data[0];
  for (luint i=0; i<_size; ++i)
    mx = (mx > data[i]) ? mx : data[i];
  return mx;
}

int GramSchmidt(Mat& A) {
  // normalize first vector:
  if (A.vectors[0].Normalize()) {
        // A_0 could not be normailized, thus Vectors are linearly dependent
        throw std::runtime_error("GramSchmidt, vectors are linearly dependent.");
    }
  auto length = A.vectors[0].Length();
  for (luint i = 1; i < A.columns; ++i) {
      for (luint j = 0; j < i; ++j) {
        A.vectors[i].LinearSum(1, -1.0 * Dot(A.vectors[i], A.vectors[j]), A.vectors[j]);
      }
      if (A.vectors[i].Normalize()) {
        // X_i could not be normailized, thus Vectors are linearly dependent
        throw std::runtime_error("GramSchmidt, vectors are linearly dependent.");
      }
    }
    return 0; // computed orthonormal basis
}
