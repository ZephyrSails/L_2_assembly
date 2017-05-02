// by: Zhiping

#include <liveness.h>

namespace L2 {
  std::set<std::string> callee_save_regs = {"r12", "r13", "r14", "r15", "rbp", "rbx"};
  std::set<std::string> caller_save_regs = {"r10", "r11", "r8", "r9", "rax", "rcx", "rdi", "rdx", "rsi"};
  std::vector<std::string> args_regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

  // utility
  void insert_item_to_set(std::set<std::string> * s, L2::Item * i) {
    if (i->type == L2::ITEM::REGISTER || i->type == L2::ITEM::VAR) {
      // if (i->name != "rsp" && i->name != "print" && i->name != "allocate" && i->name != "array-error") {
      if (i->name != "rsp") {
        s->insert(i->name);
      }
    }
  }

  void union_set(std::set<std::string> * s, std::set<std::string> * t) {
    s->insert(t->begin(), t->end());
  }

  void union_set(std::set<std::string> * s, std::vector<std::string> * t, int n) {
    if (n > t->size()) {
      n = t->size();
    }
    for (int i = 0; i < n; i++) {
      s->insert(t->at(i));
    }
  }

  std::set<std::string> minus_set(std::set<std::string> * s, std::set<std::string> * t) {
    std::set<std::string> result;
    std::set_difference(s->begin(), s->end(), t->begin(), t->end(), std::inserter(result, result.end()));
    return result;
  }

  std::map<std::string, int> build_label_map(std::vector<L2::Instruction *> instructions) {
    std::map<std::string, int> result;
    for (int k = 0; k < instructions.size(); k++) {
      L2::Instruction * i = instructions.at(k);
      if (i->type == L2::INS::LABEL_INS) {
        result[i->items.at(0)->name] = k;
      }
    }
    return result;
  }

  void gen_gen_kill(std::set<std::string> * GEN, std::set<std::string> * KILL, L2::Instruction * i) {
    switch (i->type) {
      case L2::INS::RETURN:
              GEN->insert(callee_save_regs.begin(), callee_save_regs.end());
              GEN->insert("rax");
              break;
      // case L2::INS::LABEL_INS:
      //         break;
      case L2::INS::MEM_START:
              insert_item_to_set(KILL, i->items.at(0));
              if (i->op != "<-") {
                insert_item_to_set(GEN, i->items.at(0));
              }
              insert_item_to_set(GEN, i->items.at(1));
              break;
      case L2::INS::W_START:
              insert_item_to_set(KILL, i->items.at(0));
              if (i->op != "<-") {
                insert_item_to_set(GEN, i->items.at(0));
              }
              insert_item_to_set(GEN, i->items.at(1));
              break;
      case L2::INS::CALL:
              // std::cout << "union_set1\n";
              union_set(GEN, &args_regs, i->items.at(0)->value);
              // std::cout << "insert_item_to_set\n";
              insert_item_to_set(GEN, i->items.at(0));
              // std::cout << "union_set2\n";
              union_set(KILL, &caller_save_regs);
              KILL->insert("rax");
              break;
      // case L2::INS::GOTO:
      //         break;
      case L2::INS::INC_DEC:
              insert_item_to_set(KILL, i->items.at(0));
              insert_item_to_set(GEN, i->items.at(0));
              break;
      case L2::INS::CISC:
              insert_item_to_set(KILL, i->items.at(0));
              insert_item_to_set(GEN, i->items.at(1));
              insert_item_to_set(GEN, i->items.at(2));
              break;
      case L2::INS::CMP:
              insert_item_to_set(KILL, i->items.at(0));
              insert_item_to_set(GEN, i->items.at(1));
              insert_item_to_set(GEN, i->items.at(2));
              break;
      case L2::INS::CJUMP:
              insert_item_to_set(GEN, i->items.at(0));
              insert_item_to_set(GEN, i->items.at(1));
              break;
      case L2::INS::STACK:
              insert_item_to_set(KILL, i->items.at(0));
              // KILL->insert("rsp");
              break;
      default:
              break;
    }

    std::cout << "\nGEN: ";
    for (auto reg : *GEN)
    {
        std::cout << reg << " ";
    }
    std::cout << " ///// KILL: ";
    for (auto reg : *KILL)
    {
        std::cout << reg << " ";
    }
  }

  void liveness_analysis(L2::Function *func, std::set <std::string> *GEN, std::set <std::string> *KILL, std::set <std::string> *IN, std::set <std::string> *OUT) {
    std::map<std::string, int> labelNextIndexMap = build_label_map(func->instructions);

    int n = func->instructions.size();

    int converge_count;
    while (converge_count != n) {
      converge_count = 0;

      for (int k = 0; k < n; k++) {
        std::set <std::string> newIn;
        std::set <std::string> newOut;
        L2::Instruction *cur_ins = func->instructions.at(k);

        // IN[i] = GEN[i] U (OUT[i] - KILL[i])
        union_set(&newIn, &GEN[k]);
        std::set <std::string> diff = minus_set(&OUT[k], &KILL[k]);
        union_set(&newIn, &diff);

        // OUT[i] = U (s a successor of i) IN[s]
        std::vector< int > next_indexs;

        switch (cur_ins->type) {
          case L2::INS::GOTO:
                next_indexs.push_back(labelNextIndexMap[cur_ins->items.at(0)->name]);
                break;
          case L2::INS::CJUMP:
                if (cur_ins->items.at(0)->type == L2::ITEM::NUMBER && cur_ins->items.at(1)->type == L2::ITEM::NUMBER) {
                  if ((cur_ins->op == "<=" && cur_ins->items.at(0)->value <= cur_ins->items.at(1)->value)
                    || (cur_ins->op == "<" && cur_ins->items.at(0)->value < cur_ins->items.at(1)->value)
                    || (cur_ins->op == "=" && cur_ins->items.at(0)->value == cur_ins->items.at(1)->value)) {
                      next_indexs.push_back(labelNextIndexMap[cur_ins->items.at(2)->name]);
                  } else {
                    next_indexs.push_back(labelNextIndexMap[cur_ins->items.at(3)->name]);
                  }
                } else {
                  next_indexs.push_back(labelNextIndexMap[cur_ins->items.at(2)->name]);
                  next_indexs.push_back(labelNextIndexMap[cur_ins->items.at(3)->name]);
                }
                break;
          default: // For normal situation
                next_indexs.push_back(k+1);
                break;
        }
        union_set(&newOut, &OUT[k]);
        for (int next_index : next_indexs) {
          if (next_index < n) {
            union_set(&newOut, &IN[next_index]);
          }
        }

        if (IN[k] == newIn && OUT[k] == newOut) {
          converge_count++;
        } else {
          IN[k] = newIn;
          OUT[k] = newOut;
        }
      }
    }
    ///////////////////////
    // print in & out
    ///////////////////////
    std::cout << "(\n(in\n";
    for (int k = 0; k < n; k++) {
      std::cout << "(";
      for (auto reg : IN[k]) {
          std::cout << reg << " ";
      }
      std::cout << ")\n";
    }
    std::cout << ")\n\n(out\n";
    for (int k = 0; k < n; k++) {
      std::cout << "(";
      for (auto reg : OUT[k]) {
          std::cout << reg << " ";
      }
      std::cout << ")\n";
    }
    std::cout << ")\n\n)";
  }
}
