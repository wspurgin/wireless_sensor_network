#ifndef RGG_IO_H
#define RGG_IO_H

#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>

#include "mat.h"
#include "LList.h"
#include "point.h"

vector<point> load_rgg(const char* file_path);

unordered_map<luint, LList<point*> > load_adj_list(vector<point> points, const char* file_path);

#endif
