#ifndef POINT_H
#define POINT_H

// TODO add reference pointer to place in degree lists
struct point {
  long unsigned int id;
  double x;
  double y;

  point(long unsigned int i, double j, double k):id(i),x(j),y(k){}
};

#endif
