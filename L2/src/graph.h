#pragma once

#include <L2.h>
#include <liveness.h>

namespace L2 {

  class Graph {
    std::vector< std::set < int > > neighbours;
    std::map< std::string, int > index;
    std::map< int, std::string > value;
    std::set< std::string > hold;
    // std::vector<  > color;
    std::vector< std::tuple< int, int > > order;
    // std::map < int, std::set < int > > stacked;
    std::vector< int > stack;
    std::map< int, int > color;

    int K = 15;
    int TopColor = 15;

  public:
    Graph();
    Graph(L2::Function *func);
    void add_edge(std::string var1, std::string var2);
    void add_edge(int i1, int i2);
    void add_var(std::string var);
    std::vector< std::string > neighbour_of(std::string var);

  private:
    void add_regs();
    void add_vars(std::set <std::string> VARS);
    void add_edges(std::set <std::string> VARS);
    void add_edges(std::set <std::string> VARS1, std::set <std::string> VARS2);
    void analyze(L2::Function *func);
    void print();
    void print_color();

    void coloring();
    void rebuild();
    void assign_color(int n);
    bool color_useable(int n, int c);
    void build_stack();
    void stack_node(int n);

  };

}
