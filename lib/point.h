#ifndef POINT_H
#define POINT_H

struct point {
  long unsigned int id;
  double x;
  double y;
  long unsigned int color;
  long unsigned int backbone;

  // Degree should be used in algorithms as the unmodifiable degree. The
  // curr_degree should be used in algorithms as a modifible entry on the point
  // struct. Be sure to call reset on all points to restore all temporaries
  // before applying any algorithms agains the temporary variables.
  long unsigned int degree;

  long unsigned int curr_degree;

  void reset() { curr_degree = degree; }

  point(long unsigned int i, double j, double k):id(i),x(j),y(k),degree(-1),color(0),backbone(0){
    this->reset();
  }

  // with degree as well
  point(long unsigned int i, double j, double k, double d):id(i),x(j),y(k),degree(d),color(0),backbone(0){
    this->reset();
  }

  // with degree and color
  point(long unsigned int i, double j, double k, double d, long unsigned int c):id(i),x(j),y(k),degree(d),color(c),backbone(0){
    this->reset();
  }

  // with degree and color and backbone
  point(long unsigned int i, double j, double k, double d, long unsigned int c, long unsigned int b):id(i),x(j),y(k),degree(d),color(c),backbone(b){
    this->reset();
  }

  bool operator==(const point& rhs) const { return rhs.id == this->id; }

  bool hasBackbone() const { return this->backbone != 0; }
};

#endif
