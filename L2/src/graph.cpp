// by: Zhiping

#include <graph.h>

namespace L2 {

  void Graph::add_edge(std::string var1, std::string var2) {
    neighbours[index[var1]].insert(index[var2]);
    neighbours[index[var2]].insert(index[var1]);
  }

  void Graph::add_edge(int i1, int i2) {
    neighbours[i1].insert(i2);
    neighbours[i2].insert(i1);
  }

  void Graph::add_var(std::string var) {
    int i = neighbours.size();
    index[var] = i;
    value[i] = var;

    std::set < int > nb;

    neighbours.push_back(nb);
  }

  std::vector< std::string > Graph::neighbour_of(std::string var) {
    std::vector< std::string > res;
    std::set< int >::iterator i;
    for (i = neighbours[index[var]].begin(); i != neighbours[index[var]].end(); ++i) {
        res.push_back(value[*i]);
    }
    return res;
  }

  Graph::Graph () {
    for (auto const & reg : L2::REGS) {
      this->add_var(reg);

      int newI = this->index[reg];
      for (int i = 0; i < newI; i++) {
        this->add_edge(newI, i);
      }
    }
  }

}
