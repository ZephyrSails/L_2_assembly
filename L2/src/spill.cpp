// By Zhiping

#include <spill.h>

namespace L2 {

  std::string get_spill_str(int * count, std::string * spill_target) {
    std::string spill_str = "~";
    spill_str += *spill_target;
    spill_str += std::to_string(*count);

    *count++;
    return spill_str;
  }

  void check_spill_match(L2::Instruction * rw, L2::Item * i, std::string * spill_target, int * count, int locals) {
    if (rw->items.size() > 0) {
      return;
    }
    if (i->type == L2::ITEM::VAR && i->name == *spill_target) {
      // std::cout << "hello\n";
      std::string spill_str = L2::get_spill_str(count, spill_target);
      // std::cout << "spill_str: " << spill_str << "\n";
      i->name = spill_str;

      L2::Item * dummy_var = new L2::Item();
      dummy_var->type = L2::ITEM::VAR;
      dummy_var->name = spill_str;
      dummy_var->value = -1;

      // dummy_item->value =
      L2::Item * dummy_mem = new L2::Item();
      dummy_mem->type = L2::ITEM::VAR;
      dummy_mem->name = "rsp";
      dummy_mem->value = (locals-1) * 8;

      rw->op = "<-";

      // std::cout << "rw->items0: " << rw->items.size() << "\n";

      switch (rw->type) {
        case L2::INS::W_START:    // read
          rw->items.push_back(dummy_var);
          rw->items.push_back(dummy_mem);
          break;
        case L2::INS::MEM_START:  // write
          rw->items.push_back(dummy_mem);
          rw->items.push_back(dummy_var);
          break;
      }
      // std::cout << "rw->items1: " << rw->items.size() << "\n";
    }
  }

  void spill_var(L2::Function * f, std::string * spill_target) {
    int count = 0;


    f->locals++;

    for (int k = 0; k < f->instructions.size(); k++) {

      // std::cout << "tinkering - " << k << "\n";
      L2::Instruction * i = f->instructions.at(k);
      // std::cout << "tinkering 2 - " << k << " " << *spill_target << " " << f->instructions.size() << "\n";

      L2::Instruction * read = new L2::Instruction();
      read->type = L2::INS::W_START;
      // std::cout << "read? " << read->items.size() << "\n";
      L2::Instruction * write = new L2::Instruction();
      write->type = L2::INS::MEM_START;

      // std::cout << "tinkering 3 - " << k << " " << *spill_target << "\n";

      switch (i->type) {
        // case L2::INS::RETURN:
        //         GEN->insert(callee_save_regs.begin(), callee_save_regs.end());
        //         GEN->insert("rax");
        //         break;
        // case L2::INS::LABEL_INS:
        //         break;
        case L2::INS::MEM_START:
                check_spill_match(write, i->items.at(0), spill_target, &count, f->locals);
                if (i->op != "<-") {
                  check_spill_match(read, i->items.at(0), spill_target, &count, f->locals);
                }
                check_spill_match(read, i->items.at(1), spill_target, &count, f->locals);
                break;
        case L2::INS::W_START:
                check_spill_match(write, i->items.at(0), spill_target, &count, f->locals);
                if (i->op != "<-") {
                  check_spill_match(read, i->items.at(0), spill_target, &count, f->locals);
                }
                check_spill_match(read, i->items.at(1), spill_target, &count, f->locals);
                break;
        case L2::INS::CALL:
                // union_set(GEN, &args_regs, i->items.at(0)->value);
                check_spill_match(read, i->items.at(0), spill_target, &count, f->locals);
                // union_set(KILL, &caller_save_regs);
                // KILL->insert("rax");
                break;
        // case L2::INS::GOTO:
        //         break;
        case L2::INS::INC_DEC:
                check_spill_match(read, i->items.at(0), spill_target, &count, f->locals);
                check_spill_match(write, i->items.at(0), spill_target, &count, f->locals);
                break;
        case L2::INS::CISC:
                check_spill_match(write, i->items.at(0), spill_target, &count, f->locals);
                check_spill_match(read, i->items.at(1), spill_target, &count, f->locals);
                check_spill_match(read, i->items.at(2), spill_target, &count, f->locals);
                break;
        case L2::INS::CMP:
                check_spill_match(write, i->items.at(0), spill_target, &count, f->locals);
                check_spill_match(read, i->items.at(1), spill_target, &count, f->locals);
                check_spill_match(read, i->items.at(2), spill_target, &count, f->locals);
                break;
        case L2::INS::CJUMP:
                check_spill_match(read, i->items.at(0), spill_target, &count, f->locals);
                check_spill_match(read, i->items.at(1), spill_target, &count, f->locals);
                break;
        case L2::INS::STACK:
                check_spill_match(write, i->items.at(0), spill_target, &count, f->locals);
                // KILL->insert("rsp");
                break;
        default:
                break;
      }

      // std::cout << "read? " << read->items.size() << "\n";
      // std::cout << "tinkering 2 - " << k << std::endl;
      // std::cout << read->items.size() << "read?\n";
      // std::cout << write->items.size() << "write?\n";
      if (read->items.size() > 0) {
        // std::cout << std::to_string(read->items.size()) << "read?\n";
        f->instructions.insert(f->instructions.begin()+k, read);
        k++;
      }

      if (write->items.size() > 0) {
        // std::cout << std::to_string(write->items.size()) << "write?\n";
        f->instructions.insert(f->instructions.begin()+k+1, write);
        k++;
      }


    }

  }

  void spill(L2::Function * f, std::vector< std::string > spilling_table) {
    for (int k = 0; k < spilling_table.size(); k++) {

      L2::spill_var(f, &spilling_table[k]);
    }
  }
}
