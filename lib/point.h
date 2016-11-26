#ifndef POINT_H
#define POINT_H

// TODO add reference pointer to place in degree lists
struct point {
  long unsigned int id;
  double x;
  double y;

  // Degree should be used in algorithms as the unmodifiable degree. The
  // curr_degree should be used in algorithms as a modifible entry on the point
  // struct. Be sure to call reset on all points to restore all temporaries
  // before applying any algorithms agains the temporary variables.
  long unsigned int degree;

  long unsigned int curr_degree;

  void reset() { curr_degree = degree; }

  point(long unsigned int i, double j, double k):id(i),x(j),y(k),degree(-1){
    this->reset();
  }

  // with degree as well
  point(long unsigned int i, double j, double k, double d):id(i),x(j),y(k),degree(d){
    this->reset();
  }

  bool operator==(const point& rhs) const { rhs.id == this->id; }
};

#endif
