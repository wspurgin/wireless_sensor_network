/*
 * 11/07/2016
 * © Will Spurgin
 *
 * Part 2 of CSE 7350 Algorithms Wireless Sensory Networks Project.
 * In this part we take a Random Geometric Graph (RGG) and color it. To generate
 * the optimal coloring, we implement the smallest last vertex ordering. From
 * this coloring we attempt to find the largest 4 bipartite subgraphs of the
 * RGG (these subgraphs are termed backbones). With these backbones, we want to
 * select the two that have the most edges in their maximum
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
#include <tuple>

#include "lib/mat.h"
#include "lib/LList.h"
#include "lib/Stack.h"
#include "lib/point.h"
#include "lib/rggio.h"

#define DEBUG true

int main(int argc, const char *argv[])
{
  // Guard against missing input
  if (argc < 3) {
    cerr << "Too few arguments provided." << endl;
    return 1;
  }

  vector<point> rgg = load_rgg(argv[1]);
  cout << "Retrieved RGG data points from '" << argv[1] << "'" << endl;

  luint up_to = min((int) rgg.size(), 10);

  unordered_map<luint, LList<point*> > adjacency_list = load_adj_list(rgg, argv[2]);

  cout << "Number of points: " << rgg.size() << endl << "Showing first " <<
    up_to << endl;
  for (luint i = 0; i < up_to; ++i) {
    cout << "id: " << rgg[i].id << ", x = " << rgg[i].x << ", y = " << rgg[i].y << endl;
    auto children = &adjacency_list[rgg[i].id];
    cout << '\t';
    for (auto child = children->begin(); child != children->end(); ++child)
      cout << (*child)->id << ' ';
    cout << endl;
  }

  // Create degree list

  // Index for quick access/lookup (individual
  vector<
        LList<point *>::Node<point *> *
      > placement_dg_lst;

  // Index indicates degree, value is a list of nodes with that degree.
  luint max_degree = 0;
  luint min_degree = -1;
  unordered_map<luint, Stack<point*> > degree_list;
  for(auto p = rgg.begin(); p != rgg.end(); ++p) {
    luint id = (*p).id;
    luint degree = adjacency_list[id].length();
    if (degree > max_degree) max_degree = degree;
    if (degree < min_degree) min_degree = degree;
    auto node = degree_list[degree].push(&(*p));
    placement_dg_lst.push_back(node);
  }

  // Smallest Last Degree Ordering
  vector<point *> sm_last_dg;
  for(luint i = min_degree; i <= max_degree; ++i) {
    Stack<point*> * min_dg_list = &(degree_list[i]);
    while(min_dg_list->size() > 0) {
      // Cut the current node from the current minimum degree
      auto pt = min_dg_list->pop();
      sm_last_dg.push_back(pt);
      // Update all the connect nodes to this point
      auto children = &adjacency_list[pt->id];
      for (auto child = children->begin(); child != children->end(); ++child) {
      }
    }
  }

  return 0;
}
