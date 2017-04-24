// by: Zhiping

#pragma once

#include <L2.h>
// #include <graph.h>

namespace L2 {
  void liveness_analysis(L2::Function *func, std::set <std::string> *GEN, std::set <std::string> *KILL, std::set <std::string> *IN, std::set <std::string> *OUT);
  void gen_gen_kill(std::set<std::string> * GEN, std::set<std::string> * KILL, L2::Instruction * i);
  // L2::Graph code_analysis(L2::Function *func);
}
