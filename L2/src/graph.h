#pragma once

#include <L2.h>

namespace L2 {
  class Graph {
    std::vector< std::set < int > > neighbours;
    std::map< std::string, int > index;
    std::map< int, std::string > value;

  public:
    Graph();
    void add_edge(std::string var1, std::string var2);
    void add_edge(int i1, int i2);
    void add_var(std::string var);
    std::vector< std::string > neighbour_of(std::string var);
  };
}
