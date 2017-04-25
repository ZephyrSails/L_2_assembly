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
    // std::cout << "what?";
    // if (this->hold.find(var) != this->hold.end()) {
    if (this->hold.count(var) == 0) {
      this->hold.insert(var);
      int i = neighbours.size();
      index[var] = i;
      value[i] = var;

      std::set < int > nb;

      neighbours.push_back(nb);
    }
  }

  std::vector< std::string > Graph::neighbour_of(std::string var) {
    std::vector< std::string > res;
    std::set< int >::iterator i;
    for (i = neighbours[index[var]].begin(); i != neighbours[index[var]].end(); ++i) {
        res.push_back(value[*i]);
    }
    return res;
  }

  void Graph::add_regs() {
    for (auto const & reg : L2::REGS) {
      this->add_var(reg);

      int newI = this->index[reg];
      for (int i = 0; i <= newI; i++) {
        this->add_edge(newI, i);
      }
    }
  }

  void Graph::add_vars(std::set <std::string> VARS) {
    std::set <std::string>::iterator var;
    for (var = VARS.begin(); var != VARS.end(); ++var) {
      this->add_var(*var);
    }
  }

  void Graph::add_edges(std::set <std::string> VARS) {
    this->add_edges(VARS, VARS);
  }

  void Graph::add_edges(std::set <std::string> VARS1, std::set <std::string> VARS2) {
    std::set <std::string>::iterator var1;
    for (var1 = VARS1.begin(); var1 != VARS1.end(); ++var1) {
      std::set <std::string>::iterator var2;
      for (var2 = VARS2.begin(); var2 != VARS2.end(); ++var2) {
        this->add_edge(*var1, *var2);
      }
    }
  }

  Graph::Graph() {
    this->add_regs();
  }

  Graph::Graph(L2::Function *func) {
    // 1 node per variable
    // Registers are considered variables
    // Connect a register to all other registers (even those not used by f)
    this->add_regs();
    this->analyze(func);
  }

  void Graph::analyze(L2::Function *func) {
    int n = func->instructions.size();

    std::set <std::string> GEN[n];
    std::set <std::string> KILL[n];

    for (int k = 0; k < n; k++) {
      L2::Instruction *i = func->instructions.at(k);

      L2::gen_gen_kill(&GEN[k], &KILL[k], i);
    }

    std::set <std::string> IN[n];
    std::set <std::string> OUT[n];

    L2::liveness_analysis(func, GEN, KILL, IN, OUT);

    for (int k = 0; k < n; k++) {
      this->add_vars(IN[k]);
      this->add_vars(OUT[n]);

      // Connect each pair of variables that belong to the same IN or OUT set
      this->add_edges(IN[k]);
      this->add_edges(OUT[k]);

      // Connect variables in KILL[i] with those in OUT[i], Unless it is (x <- y)
      if (func->instructions[k]->op != "<-") {
        this->add_edges(KILL[k], OUT[k]);
      }
      // Handle constrained arithmetic via extra edges
      if (func->instructions[k]->op == "<<=" || func->instructions[k]->op == ">>=") {
        for (auto const & reg : L2::REGS) {
          if (reg != "rcx") {
            this->add_edge(func->instructions[k]->items[1]->name, reg);
          }
        }
      }
    }

    this->print();
  }

  void Graph::print() {
    for (int k = 0; k < this->neighbours.size(); k++) {
    // for (auto const & nbs : this->neighbours) {
      std::cout << this->value[k] << ": ";
      std::set<int>::iterator iter;
      for(iter = neighbours[k].begin(); iter != neighbours[k].end(); ++iter) {
        std::cout << this->value[*iter] << " ";
      }
      std::cout << "\n";
    }
  }
}
