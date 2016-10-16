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

#include "lib/mat.h"
#include "lib/vec.h"

#define DEBUG true

using namespace std;

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

Mat gen_random_plane(unsigned int num_nodes) {
  Mat net = Mat(num_nodes, 2);
  net.Random();
  return net;
}

Mat gen_random_disk(unsigned int num_nodes) {
  Mat net = Mat(num_nodes, 2);
  net.RandomDisk();
  return net;
}

int main(int argc, const char *argv[])
{
  if (argc < 3)
    cerr << error("Error: Too few inputs to generation.") << endl <<
      '\t' << help() << endl;

  unsigned int num_nodes = atoi(argv[1]);
  double avg_degree      = atof(argv[2]);
  bool as_plane             = true;

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
  double radius = sqrt(avg_degree / num_nodes / M_PI);

  if (DEBUG)
    cout  <<  "num_nodes=" << num_nodes << ";avg_degree=" << avg_degree <<
      ";as_plane=" << as_plane << ";radius=" << radius <<  endl;

  stringstream file_name_s;
  if (as_plane) {
    rgg = gen_random_plane(num_nodes);
    file_name_s << "plane";
  }
  else {
    rgg = gen_random_disk(num_nodes);
    file_name_s << "disk";
  }

  file_name_s << '_' << num_nodes << '_' << avg_degree << ".csv";
  // TODO generate adjacency list by estimating radius of encounter / avg degree
  string output_file = file_name_s.str();
  rgg.Write(output_file.c_str(), ',');

  return 0;
}
