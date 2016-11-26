#include "rggio.h"

using namespace std;

vector<point> load_rgg(const char* file_path) {
  vector<point> loader;

  // Ensure the file exists
  fstream fin(file_path);

  if (fin.is_open()) {
    char buffer[200];
    luint id = 0;
    while(fin.good()) {
      fin.getline(buffer, 200, ',');
      // After initial read, break if eof or errors before going fruther
      if (!fin.good()) break;

      double x = atof(buffer);

      fin.getline(buffer, 200);
      double y = atof(buffer);

      loader.push_back(point(id, x, y));
      id++;
    }
    return loader;
  }
  else {
    cerr << "Could not open file " << file_path << endl;
  }
  return loader;
}

unordered_map<luint, LList<point*> > load_adj_list(vector<point>& points, const char* file_path) {
  unordered_map<luint, LList<point*> > adj_list;

  fstream fin(file_path);

  if (fin.is_open()) {
    while(fin.good()) {
      string buffer;
      getline(fin, buffer, ':');
      // After initial read, break if eof or errors before going fruther
      if (!fin.good()) break;

      luint id = stoul(buffer);

      string children;
      getline(fin, children);
      stringstream ss(children);
      for(string child; getline(ss, child, ' ');) {
        luint child_id = stoul(child);
        // append to ensure order
        adj_list[id].append(&points[child_id]);
      }
    }
  }
  else {
    cerr << "Could not open file " << file_path << endl;
  }

  for (auto i = points.begin(); i != points.end(); ++i) {
    i->degree = (long unsigned int) adj_list[i->id].length();
    i->reset();
  }
  return adj_list;
}
