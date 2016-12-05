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
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <utility>
#include <array>
#include <set>

#include "lib/mat.h"
#include "lib/LList.h"
#include "lib/Stack.h"
#include "lib/Queue.h"
#include "lib/point.h"
#include "lib/rggio.h"

#define DEBUG true

namespace std {
  template<> struct hash<pair<luint,luint> > {
    size_t operator()(const pair<luint, luint>& p) const {
      return hash<luint>()(p.first) ^ hash<luint>()(p.second);
    }
  };
}

using namespace std;

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


  /*
   * Smallest Last Ordering
   */

  // degree-when-deleted stats vector
  vector<pair<luint, luint> > degree_when_deleted;

  // Create degree list
  // Index for quick access/lookup of (individual placement in list)
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

    // Find first non-empty smallest degree list
    luint curr_min_degree;
    for (auto i = 0; i < max_degree; ++i) {
      if (degree_list[i].size() > 0) {
        curr_min_degree = i;
        break;
      }
    }

    // Theres a space at the end of output so the buffer flushes correctly
    array<char, 5> timer_chars{ ' ', '|', '/', '-', '\\' };
    cout << '\r' << timer_chars[(j - 1) % timer_chars.size()] << " j: " << j
      << ' ' << "curr_min_degree: " << curr_min_degree << ' ';


    // Cut the current node from the current minimum degree
    auto pt = degree_list[curr_min_degree].pop();
    sm_last_dg.insert(pt);

    // Add statistic of what degree at this iteration was deleted. When we start
    // plunging in this distribution, then it indicates a terminal clique size.
    degree_when_deleted.push_back(pair<luint, luint>(curr_min_degree, pt->degree));

    // Update all the connecting nodes to the point (pt) (i.e. H - v_j)
    auto children = &adjacency_list[pt->id];
    for (auto child = children->begin(); child != children->end(); ++child) {
      // Update Children adjacency to remove the current point (pt) (i.e. v_j).
      auto node = placement_dg_lst[(*child)->id];
      (&adjacency_list[(*child)->id])->remove(pt);

      // Retrieve the current degree of this child
      luint degree = (*child)->curr_degree;

      // Remove the child from its spot in the (curr_degree) degree list. Then
      // add it to the (curr_degree-1) degree list. Lastly, update the lookup
      // table for the new child node.
      point* child_pt = degree_list[degree].remove(LList<point*>::iterator(node));
      auto child_new_node = degree_list[degree-1].push(child_pt);
      placement_dg_lst[(*child)->id] = child_new_node;

      // Finally subtract 1 from the degree of the current child
      (*child)->curr_degree--;
    }

    // Erase current node (i.e. v_j) from current adjacency list (i.e. H)
    adjacency_list.erase(pt->id);
  }


  cout << endl << endl << "Smallest Last Ordering: (showing " << up_to << " entries)"
    << endl;

  for (luint i = 0; i < up_to; ++i) {
    cout << i << " - id: " << sm_last_dg[i]->id << endl;
  }

  // reset adjacency list
  adjacency_list = original_adj;

  // Coloring from this vertex ordering, we need to color the graph points
  luint base_color = 1;
  luint max_color = base_color;
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
    if (curr_color > max_color)
      max_color = curr_color;

    // Finally, assign the first available color.
    (*v)->color = curr_color;
  }


  cout << endl << "Coloring of points: (showing " << up_to << " entries)"
    << endl;

  for (luint i = 0; i < up_to; ++i) {
    cout << "id : " << sm_last_dg[i]->id << " - color: " << sm_last_dg[i]->color
      << endl;
  }

  /* Find δ + 1 largest independent sets, (where δ is the minimum degree).
   * To find the largest independent sets, we simply have to find the δ + 1
   * lagest color classes. However, the requirement states that we only need to
   * find the first largest 4. If we found the largest δ + 1, then we can
   * guarantee maximum coverage by the bipartite backbone.
   */

  // Uncomment the following to actually find the δ + 1 largest color classes
  // luint num_ind_sets = min_degree + 1;
  luint num_ind_sets = 4;

  // Gaurd in case fewer than 4 color classes.
  num_ind_sets = min(num_ind_sets, max_color);


  set<luint> largest_color_classes;
  vector<luint> color_class_freq(max_color+1);
  vector<pair<luint, luint>> colors_by_freq;

  for (auto v = rgg.begin(); v < rgg.end(); ++v)
    color_class_freq[v->color]++;

  // cout << "\tColor\t|\t Frequency" << endl;
  for (luint c = 1; c <= max_color; ++c) {
    colors_by_freq.push_back(make_pair(c, color_class_freq[c]));
    // cout << '\t' << c << "\t|\t" << color_class_freq[c] << endl;
  }

  auto comp = [](pair<luint, luint> a, pair<luint, luint> b) { return a.second < b.second; };
  sort(colors_by_freq.begin(), colors_by_freq.end(), comp);

  for (luint i = 0; i < num_ind_sets; ++i) {
    largest_color_classes.insert(colors_by_freq.back().first);
    colors_by_freq.pop_back();
  }

  cout << endl << "Top " << num_ind_sets << " color classes:" << endl;

  for (auto c = largest_color_classes.begin(); c != largest_color_classes.end(); ++c)
    cout << "color: " << *c << " - freq: " << color_class_freq[*c] << endl;

  /*
   * Backbone Selection
   */
  luint total_edge_count = 0;
  for (auto i = adjacency_list.begin(); i != adjacency_list.end(); ++i)
    total_edge_count += i->second.length();

  // Form all combinations of the largest color classes
  vector<pair<luint, luint> > combinations;
  for (auto c = largest_color_classes.begin(); c != largest_color_classes.end(); ++c)
    for (auto k = next(c); k != largest_color_classes.end(); ++k)
      combinations.push_back(make_pair(*c, *k));

  // vector with pair of the combination (pair) - edge_count
  unordered_map< pair<luint, luint>, luint> backbone_by_edge_count;
  unordered_map< pair<luint, luint>, LList<point *> > backbone_max_subgraph_nodes;
  for (auto backbone : combinations) {
    luint c, k;
    tie(c, k) = backbone;
    backbone_by_edge_count[backbone] = 0;

    for (auto h_i : adjacency_list) {
      luint node_id = h_i.first;

      // Only consider starting points that are in the backbone colors as
      // starting nodes
      if (rgg[node_id].color != c && rgg[node_id].color != k)
        continue;

      set<luint> visited;
      Queue<luint> node_queue;
      LList<point *> backbone_nodes;
      node_queue.push(node_id);
      backbone_nodes.insert(&rgg[node_id]);

      // determine how many edges we can reach from this starting backbone
      // starting node.
      luint num_edges = 0;
      luint j = 0;
      while(node_queue.size() > 0) {
        cout << "\r j = " << j++ << " queue size: " << node_queue.size();

        // get current id
        luint curr_id = node_queue.pop();
        if (visited.count(curr_id) == 1) // we've already visited this node
          continue;

        // if the current node has one of the backbone colors, then we'll add
        // it to the nodes
        if (rgg[curr_id].color == c || rgg[curr_id].color == k)
          backbone_nodes.insert(&rgg[curr_id]);

        auto children = adjacency_list[curr_id];
        num_edges += children.length();
        for (auto child = children.begin(); child != children.end(); ++child) {
          // if we haven't visitied the node
          if (visited.count((*child)->id) == 0)
            node_queue.push((*child)->id);
        }

        // Finally, mark the vertex as visited
        visited.insert(curr_id);
      }

      if (num_edges > backbone_by_edge_count[backbone]) {
        backbone_by_edge_count[backbone] = num_edges;
        backbone_max_subgraph_nodes[backbone] = backbone_nodes;
      }
    }

    // Since each edge is recorded twice in the adjacency list, we should divide
    // by two.
    backbone_by_edge_count[backbone] /= 2;
  }

  // Same as with above, we'll normalize the total edge count to the count of
  // unique edges.
  total_edge_count /= 2;

  cout << endl << "Reporting stats of Bipartite Backbone options:" << endl;
  cout << "Total # of edges: " << total_edge_count << endl;

  for (auto backbone : combinations) {
    cout << "Backbone Colors (" << backbone.first << ',' << backbone.second
      << ')' << " - Num Edges Reachable: " << backbone_by_edge_count[backbone]
      << " - As \% of Total Edges: "
      << (double) backbone_by_edge_count[backbone] / (double) total_edge_count * 100
      << '%' << endl;
  }

  // Write out smallest last ordering, and coloring
  stringstream base_file_s, graph_output_file_s, degree_when_deleted_s;
  string buffer(argv[1]);
  base_file_s << buffer.substr(0, strstr(argv[1], ".") - argv[1]);
  graph_output_file_s << base_file_s.str() << "_graph.csv";
  degree_when_deleted_s << base_file_s.str() << "_degree_when_deleted.csv";

  fstream fout;
  fout.open(graph_output_file_s.str().c_str(), ios_base::out | ios_base::trunc);
  fout << "id,x,y,degree,color,backbone" << endl;
  for (auto v = sm_last_dg.begin(); v != sm_last_dg.end(); ++v) {
    auto point = (*v);
    fout << point->id << ',' << point->x << ',' << point->y << ','
      << point->degree << ',' << point->color << ',';
    if (point->hasBackbone())
      fout << point->backbone;
    fout << endl;
  }
  fout.close();

  cout << endl << graph_output_file_s.str() << " written out." << endl;

  fout.open(degree_when_deleted_s.str().c_str(), ios_base::out | ios_base::trunc);
  fout << "iteration,degree_when_deleted,original_degree" << endl;
  for (luint i = 0; i < degree_when_deleted.size(); ++i) {
    fout << i << ',' << degree_when_deleted[i].first << ',' << degree_when_deleted[i].second << endl;
  }
  fout.close();

  cout << degree_when_deleted_s.str() << " written out." << endl;

  return 0;
}
