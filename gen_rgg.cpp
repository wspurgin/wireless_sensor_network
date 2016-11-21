/*
 * 10/15/2016
 * © Will Spurgin
 *
 * Part 1 of CSE 7350 Algorithms Wireless Sensory Networks Project.
 * In this part we generate unifromly random distributions of nodes (2D points)
 * on a plane (unit square) and a disk (unit circle). Input arguments are given
 * via commandline. Example Usage
 *
 * gen_rgg <num_nodes> <average_degree> [plane|disk]
 *
 * ./gen_rgg 6400 80 plane
 *
 * Note that the default geometric surface is a plane.
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <unordered_map>
#include <vector>

#include "lib/mat.h"
#include "lib/LList.h"
#include "lib/point.h"

#define DEBUG true

using namespace std;

// see http://stackoverflow.com/a/12827233/2639561
string WARN = "\x1b[33m";
string ERRO = "\x1b[31m";
string INFO = "\x1b[34m";
string SUCC = "\x1b[32m";
string ENDC = "\x1b[0m";

const string error(const string message) {
  stringstream ss;
  ss << ERRO << message << ENDC;
  return ss.str();
}

const string warning(const string message) {
  stringstream ss;
  ss << WARN << message << ENDC;
  return ss.str();
}

const char* help() {
  return "Usage: ./gen_rgg <num_nodes> <average_degree> [plane|disk]";
}

Mat gen_random_plane(luint num_nodes) {
  Mat net = Mat(num_nodes, 2);
  net.Random();
  return net;
}

Mat gen_random_disk(luint num_nodes) {
  Mat net = Mat(num_nodes, 2);
  net.RandomDisk();
  return net;
}


// Vector points will be filled
unordered_map<luint, LList<point*> > build_adjacency(const Mat& rgg, double avg_degree, double radius, vector<point>& points) {
  unordered_map<luint, LList<point*> > adjacency_list;

  for(luint i = 0; i < rgg.Rows(); ++i)
    points.push_back(point(i, rgg(i, 0), rgg(i, 1)));

  sort(points.begin(), points.end(), [](point a, point b) {
        return a.x < b.x;
      });

  // Sweep approach
  auto vectorized_points = rgg.T();
  auto window_start = points.begin();
  luint num_edges = 0;
  for(auto i = points.begin() + 1; i != points.end(); ++i) {
    adjacency_list[i->id];
    // first, should the window be moved?
    while (abs(i->x - window_start->x) > radius && i - 1 > window_start)
      ++window_start;
    for(auto j = window_start; j != i; ++j) {
      // calculate the distince between the points
      Vec diff = vectorized_points.AccessColumn(i->id) - vectorized_points.AccessColumn(j->id);
      if (diff.TwoNorm() <= radius) {
        num_edges++;
        adjacency_list[i->id].insert(&*j);
        adjacency_list[j->id].insert(&*i);
      }
    }
  }
  if (DEBUG) cout << "Number of Edges = " << num_edges << endl;
  return adjacency_list;
}

int main(int argc, const char *argv[])
{
  if (argc < 3)
    cerr << error("Error: Too few inputs to generation.") << endl <<
      '\t' << help() << endl;

  luint num_nodes   = atoi(argv[1]);
  double avg_degree = atof(argv[2]);
  bool as_plane     = true;

  // Handle shape argument
  if (argc > 3) {
    string geo_shape = argv[3];
    transform(geo_shape.begin(), geo_shape.end(), geo_shape.begin(), ::tolower);
    if (geo_shape == "disk")
      as_plane = false;
    else if (geo_shape == "plane")
      as_plane = true; // just for completeness
    else
      cerr << WARN << "Warning: \x1b[1;4m" << argv[3] << "\x1b[0;33m is not a "
        << "valid chioce of geometric surface" << endl << '\t' << help() <<
        ENDC << endl;
  }


  // Initialization is complete
  Mat rgg;
  double radius;
  stringstream file_base_name_s;

  // Generate randomized points and calculated radius argument to
  // build_adjacency
  if (as_plane) {
    rgg = gen_random_plane(num_nodes);
    file_base_name_s << "plane";
    radius = sqrt(avg_degree / num_nodes / M_PI);
  }
  else {
    rgg = gen_random_disk(num_nodes);
    file_base_name_s << "disk";
    radius = sqrt(avg_degree / num_nodes);
  }

  if (DEBUG)
    cout  <<  "num_nodes=" << num_nodes << ";avg_degree=" << avg_degree <<
      ";as_plane=" << as_plane << ";radius=" << radius <<  endl;

  // Build adjacency list from generated data and desired radius (the estimated
  // radius to achived the average degree)
  vector<point> points;
  auto adjacency_list = build_adjacency(rgg, avg_degree, radius, points);

  // Calculate various stats around degree.
  double actual_avg_degree = 0.0;
  luint min_degree = -1;
  luint max_degree = 0;
  vector<luint> dist_degree(num_nodes);

  for(auto i : adjacency_list) {
    auto degree = i.second.length();
    actual_avg_degree += degree;
    dist_degree[degree]++;
    if (degree > max_degree) max_degree = degree;
    if (degree < min_degree) min_degree = degree;
  }
  actual_avg_degree /= num_nodes;

  if (DEBUG) {
    cout << "Estimated average degree = " << avg_degree << endl <<
      "Actual average degree = " << actual_avg_degree << endl;
    cout << "Max Degree = " << max_degree << endl << "Min Degree = " <<
      min_degree << endl;
  }


  // Output data files
  file_base_name_s << '_' << num_nodes << '_' << avg_degree;
  string output_file_base_name = file_base_name_s.str();
  string rgg_output_file = output_file_base_name + ".csv";
  string adjacency_list_output_file = output_file_base_name + "_adj_list.txt";
  string degree_dist_output_file = output_file_base_name + "_degree_dist.csv";
  rgg.Write(rgg_output_file.c_str(), ',');

  fstream fout;
  fout.open(adjacency_list_output_file, ios_base::out | ios_base::trunc);
  for(auto i : adjacency_list) {
    fout << i.first << ":";
    for(auto j : i.second)
      fout << j->id << ' ';
    fout << endl;
  }
  fout.close();

  fout.open(degree_dist_output_file, ios_base::out | ios_base::trunc);
  for(luint i = 0; i <= max_degree; ++i)
    fout << i << ',' << dist_degree[i] << endl;

  return 0;
}
