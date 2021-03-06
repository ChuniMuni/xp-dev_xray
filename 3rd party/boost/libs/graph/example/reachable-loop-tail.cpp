//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee, 
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, Indiana University,
// Bloomington, IN 47405.
//
// Permission to modify the code and to distribute the code is
// granted, provided the text of this NOTICE is retained, a notice if
// the code was modified is included with the above COPYRIGHT NOTICE
// and with the COPYRIGHT NOTICE in the LICENSE file, and that the
// LICENSE file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
//=======================================================================
#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/reverse_graph.hpp>

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    std::cerr << "usage: reachable-loop-tail.exe <in-file> <out-file>"
      << std::endl;
    return -1;
  }
  using namespace boost;
  GraphvizDigraph g_in;
  read_graphviz(argv[1], g_in);

  typedef adjacency_list < vecS, vecS, bidirectionalS,
    GraphvizVertexProperty,
    GraphvizEdgeProperty, GraphvizGraphProperty > Graph;
  Graph g;
  copy_graph(g_in, g);

  graph_traits < GraphvizDigraph >::vertex_descriptor loop_tail = 6;
  typedef color_traits < default_color_type > Color;
  default_color_type c;

  std::vector < default_color_type > reachable_to_tail(num_vertices(g));
  reverse_graph < Graph > reverse_g(g);
  depth_first_visit(reverse_g, loop_tail, default_dfs_visitor(),
                    make_iterator_property_map(reachable_to_tail.begin(),
                                               get(vertex_index, g), c));

  std::ofstream loops_out(argv[2]);
  loops_out << "digraph G {\n"
    << "  graph [ratio=\"fill\",size=\"3,3\"];\n"
    << "  node [shape=\"box\"];\n" << "  edge [style=\"bold\"];\n";

  property_map<Graph, vertex_attribute_t>::type
    vattr_map = get(vertex_attribute, g);
  graph_traits < GraphvizDigraph >::vertex_iterator i, i_end;
  for (tie(i, i_end) = vertices(g_in); i != i_end; ++i) {
    loops_out << *i << "[label=\"" << vattr_map[*i]["label"]
      << "\"";
    if (reachable_to_tail[*i] != Color::white()) {
      loops_out << ", color=\"gray\", style=\"filled\"";
    }
    loops_out << "]\n";
  }
  graph_traits < GraphvizDigraph >::edge_iterator e, e_end;
  for (tie(e, e_end) = edges(g_in); e != e_end; ++e)
    loops_out << source(*e, g) << " -> " << target(*e, g) << ";\n";
  loops_out << "}\n";
  return EXIT_SUCCESS;
}
