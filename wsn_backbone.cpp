/*
 * 11/07/2016
 * © Will Spurgin
 *
 * Part 2 of CSE 7350 Algorithms Wireless Sensory Networks Project.
 * In this part we take a Random Geometric Graph (RGG) and color it. To generate
 * the optimal coloring, we implement the smallest last vertex ordering. From
 * this coloring we attempt to find the largest 4 bipartite subgraphs of the
 * RGG (these subgraphs are termed backbones). With these backbones, we want to
 * select the two that have the most edges.
 *
 * ./wsn_backbone rgg_points.csv rgg_adjacency_list.txt
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
#include <utility>
#include <array>
#include <set>

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

  auto original_adj = adjacency_list;

  cout << "Number of points: " << rgg.size() << endl << "Showing first " <<
    up_to << endl;
  for (luint i = 0; i < up_to; ++i)
    cout << "id: " << rgg[i].id << ", x = " << rgg[i].x << ", y = " << rgg[i].y << endl;
  cout << endl;

  // Create degree list
  // Index for quick access/lookup (individual
  unordered_map<luint,
        LList<point *>::Node<point *> *
      > placement_dg_lst;

  // Index indicates degree, value is a list of nodes with that degree.
  luint max_degree = 0;
  luint min_degree = -1;
  unordered_map<luint, Stack<point*> > degree_list;
  for(auto p = rgg.begin(); p != rgg.end(); ++p) {
    luint degree = (*p).degree;
    if (degree > max_degree) max_degree = degree;
    if (degree < min_degree) min_degree = degree;
    auto node = degree_list[degree].push(&(*p));
    placement_dg_lst[node->data_->id] = node;
  }

  // Smallest Last Degree Ordering
  LList<point *> sm_last_dg;
  for(luint j = rgg.size(); j >= 1; --j) {
    Stack<point*> * min_dg_list;

    // Find first non-empty smallest degree list
    luint curr_min_degree;
    for (auto i = 0; i < max_degree; ++i) {
      if (degree_list[i].size() > 0) {
        curr_min_degree = i;
        min_dg_list = &(degree_list[i]);
        break;
      }
    }

    array<char, 5> timer_chars{ ' ', '|', '/', '-', '\\' };
    cout << '\r' << timer_chars[(j - 1) % timer_chars.size()] << " j: " << j << ' '
      << "curr_min_degree: " << curr_min_degree << ' ' << "size: "
      << min_dg_list->size();

    // Cut the current node from the current minimum degree
    auto pt = min_dg_list->pop();
    sm_last_dg.insert(pt);

    // Update all the connecting nodes to the point (pt) (i.e. H - v_j)
    auto children = &adjacency_list[pt->id];
    for (auto child = children->begin(); child != children->end(); ++child) {
      // Update Children adjacency to remove the current point (pt) (i.e. v_j).
      auto node = placement_dg_lst[(*child)->id];
      (&adjacency_list[(*child)->id])->remove(pt);

      // Retrieve the current degree of this child
      luint degree = node->data_->curr_degree;

      // Remove the child from its spot in the i-th degree list. Then add it to
      // the (i-1) degree list. Lastly, update the lookup table for the new
      // child node.
      point* child_pt = (&degree_list[degree])->list()->remove(LList<point*>::iterator(node));
      auto child_new_node = (&degree_list[degree-1])->push(child_pt);
      placement_dg_lst[(*child)->id] = child_new_node;

      // Finally subtract 1 from the degree of the current child
      node->data_->curr_degree--;
    }

    // Erase current node (i.e. v_j) from current adjacency list (i.e. H)
    adjacency_list.erase(pt->id);
  }


  cout << endl << "Smallest Last Ordering: (showing " << up_to << " entries)"
    << endl;

  for (luint i = 0; i < up_to; ++i) {
    cout << i << " - id: " << sm_last_dg[i]->id << endl;
  }

  // reset adjacency list
  adjacency_list = original_adj;

  // Coloring from this vertex ordering, we need to color the graph points
  luint base_color = 1;
  for (auto v = sm_last_dg.begin(); v != sm_last_dg.end(); ++v) {
    luint curr_color = base_color;

    // Collect the neighboorhood of colors, insert for a set is logorithmic in
    // the size of the container.
    set<luint> neighborhood_colors;
    auto children = adjacency_list[(*v)->id];
    for (auto i = children.begin(); i != children.end(); ++i) {
      neighborhood_colors.insert((*i)->color);
    }

    // We'll start by checking if we can assign the current color and continue
    // incrementing the color value by one till we can assign the color. In this
    // way, we will always assign the lowest possible color.
    //
    // The std::set is typically implemented as a red-black tree, so search time
    // is logorithmic in the size of the set. So in this case its O(lg |E_v|)
    // where |E_v| is the number of edges. In reality, since there are no
    // duplicates in a set, it can be less than lg(|E_v|).
    while(neighborhood_colors.find(curr_color) != neighborhood_colors.end())
      curr_color++;

    // Finally, assign the first available color.
    (*v)->color = curr_color;
  }


  cout << endl << "Coloring of points: (showing " << up_to << " entries)"
    << endl;

  for (luint i = 0; i < up_to; ++i) {
    cout << "id : " << sm_last_dg[i]->id << " - color: " << sm_last_dg[i]->color
      << endl;
  }

  return 0;
}
