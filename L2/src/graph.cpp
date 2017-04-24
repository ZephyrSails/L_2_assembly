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
    this->add_regs();
  }

  Graph::Graph (L2::Function *func) {
    this->add_regs();
    this->analyze(func);
  }

  void Graph::add_regs() {
    for (auto const & reg : L2::REGS) {
      this->add_var(reg);

      int newI = this->index[reg];
      for (int i = 0; i < newI; i++) {
        this->add_edge(newI, i);
      }
    }
  }

  void Graph::analyze(L2::Function *func) {
    int n = func->instructions.size();

    std::set<std::string> GEN[n];
    std::set<std::string> KILL[n];

    for (int k = 0; k < n; k++) {
      L2::Instruction *i = func->instructions.at(k);

      L2::gen_gen_kill(&GEN[k], &KILL[k], i);
      // We need to build GEN and KILL here
    }

    std::set <std::string> IN[n];
    std::set <std::string> OUT[n];

    L2::liveness_analysis(func, GEN, KILL, IN, OUT);

    // TODO:
    // 1 node per variable
    // Registers are considered variables
    // Connect each pair of variables that belong to the same IN or OUT set
    // Connect a register to all other registers (even those not used by f)
    // Connect variables in KILL[i] with those in OUT[i]
    // Unless it is (x <- y)
    // Handle constrained arithmetic via extra edges
  }

}
