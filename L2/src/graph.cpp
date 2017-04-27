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

  Graph::Graph() {}

  Graph::Graph(L2::Function *func, int K) {
    // 1 node per variable
    // Registers are considered variables
    // Connect a register to all other registers (even those not used by f)
    this->add_regs();
    this->analyze(func);

    this->K = K;
    this->TopColor = K;
    // this->coloring();
  }

  void Graph::analyze(L2::Function *func) {
    // std::cout << "start analyze(), building GEN, KILL\n";
    int n = func->instructions.size();

    std::set <std::string> GEN[n];
    std::set <std::string> KILL[n];

    for (int k = 0; k < n; k++) {
      L2::Instruction *i = func->instructions.at(k);
      // std::cout << "gen_gen_kill():" << func->instructions[k]->items[0]->name << " " << func->instructions[k]->op << "\n";
      L2::gen_gen_kill(&GEN[k], &KILL[k], i);
    }

    // std::cout << "building IN, OUT\n";
    std::set <std::string> IN[n];
    std::set <std::string> OUT[n];

    L2::liveness_analysis(func, GEN, KILL, IN, OUT);

    // std::cout << "adding regs\n";
    for (int k = 0; k < n; k++) {
      for (int j = 0; j < func->instructions[k]->items.size(); j++) {
        L2::Item * i = func->instructions[k]->items[j];
        if (i->type == L2::ITEM::VAR) {
          this->add_var(i->name);
        }
      }

      this->add_vars(IN[k]);
      this->add_vars(OUT[k]);

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

  void Graph::print_color() {
    // std::map<Key, Value> m { ... /* initialize it */ ... };

    for (const auto &p : this->color) {
        std::cout << "color[" << this->value[p.first] << "] = " << p.second << '\n';
    }
  }

  void Graph::stack_node(int n) {
    //// map < int, set >: who has been deleted, and their edges
    // set < int > stacked: who has been deleted
    // vector < int > stack:

    // this->stacked.insert(n);
    this->stack.push_back(n);
  }

  void Graph::build_stack() {
    // vector < tuple > order: [(,), (,), |  (,), (,)]
    for (int k = 0; k < this->neighbours.size(); k++) {
      std::tuple< int, int > curOrder = {this->neighbours[k].size(), k};
      this->order.push_back(curOrder);
    }
    // std::cout << "mark node by neighbours done";
    std::sort(this->order.begin(), this->order.end());
    // std::cout << "sort node by neighbours done";

    int indexK = this->order.size() - 1;
    for (int k = 0; k < this->order.size(); k++) {
      if (std::get<0>(this->order[k]) >= this->K) {
        indexK = k - 1;
      }
    }
    // std::cout << "sort node by neighbours done";

    for (int k = indexK; k >= 0; k--) {
      this->stack_node(std::get<1>(this->order[k]));
    }

    for (int k = this->order.size() - 1; k > indexK; k--) {
      this->stack_node(std::get<1>(this->order[k]));
    }
  }

  bool Graph::color_useable(int n, int c) {
    std::set< int >::iterator i;
    for (i = neighbours[n].begin(); i != neighbours[n].end(); ++i) {
      if ((*i < this->K && this->color[*i] == c)
          || (this->color.count(*i) == 1 && this->color[*i] == c)) {
        return false;
      }
    }
    return true;
  }

  void Graph::assign_color(int n) {
    if (n < this->K) {
      return;
    }

    for (int c = 0; c < this->TopColor; c++) {
      if (this->color_useable(n, c)) {
        this->color[n] = c;
        return;
      }
    }
    this->color[n] = this->TopColor;
    this->TopColor++;
  }

  void Graph::rebuild() {
    // map < int, int > color
    // vector < int > color
    for (int k = 0; k < this->K; k++) {
      this->color[k] = k;
    }

    for (int k = this->stack.size()-1; k >= 0; k--) {
      this->assign_color(this->stack[k]);
    }
  }

  std::vector< std::string > Graph::coloring() {

    this->build_stack();
    this->rebuild();

    // this->print();
    // this->print_color();

    std::vector< std::string > spilling_table;

    for (const auto &p : this->color) {
      if (p.second >= this->K) {
        spilling_table.push_back(this->value[p.first]);
      }
    }
    return spilling_table;
  }

  std::string Graph::get_color(std::string reg) {
    if (reg == "rsp") {
      return "rsp";
    }
    return this->value[this->color[this->index[reg]]];
  }
}
