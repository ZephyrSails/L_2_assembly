#pragma once

// Used to do:
//   code analyze.
//   coloring.

#include <L2.h>
#include <liveness.h>

namespace L2 {

  class Graph {

  public:
    Graph();
    Graph(L2::Function *func, int K);
    std::vector< std::string > coloring();

    std::string get_color(std::string reg);

  private:
    int K;
    int TopColor;

    std::vector< std::set < int > > neighbours;
    std::map< std::string, int > index;
    std::map< int, std::string > value;
    std::set< std::string > hold; // to avoid mutiple insert

    std::vector< std::tuple< int, int > > order;
    // std::map < int, std::set < int > > stacked;
    std::vector< int > stack;
    std::map< int, int > color;
    // basic
    void add_edge(std::string var1, std::string var2);
    void add_edge(int i1, int i2);
    void add_var(std::string var);
    std::vector< std::string > neighbour_of(std::string var);
    // build graph
    void add_regs();
    void add_vars(std::set <std::string> VARS);
    void add_edges(std::set <std::string> VARS);
    void add_edges(std::set <std::string> VARS1, std::set <std::string> VARS2);
    void analyze(L2::Function *func);
    void print();
    void print_color();
    // coloring
    void rebuild();
    void assign_color(int n);
    bool color_useable(int n, int c);
    void build_stack();
    void stack_node(int n);
  };

}
