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
      std::string spill_str = L2::get_spill_str(count, spill_target);
      i->name = spill_str;

      L2::Item * dummy_var = new L2::Item();
      dummy_var->type = L2::ITEM::VAR;
      dummy_var->name = spill_str;
      dummy_var->value = -1;

      L2::Item * dummy_mem = new L2::Item();
      dummy_mem->type = L2::ITEM::VAR;
      dummy_mem->name = "rsp";
      dummy_mem->value = (locals-1) * 8;

      rw->op = "<-";

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
    }
  }

  void spill_var(L2::Function * f, std::string * spill_target) {
    int count = 0;

    f->locals++;

    for (int k = 0; k < f->instructions.size(); k++) {

      L2::Instruction * i = f->instructions.at(k);

      L2::Instruction * read = new L2::Instruction();   // read from stack
      L2::Instruction * write = new L2::Instruction();  // write to stack
      read->type = L2::INS::W_START;
      write->type = L2::INS::MEM_START;

      switch (i->type) {
        // case L2::INS::RETURN:
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
                check_spill_match(read, i->items.at(0), spill_target, &count, f->locals);
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
                break;
        default:
                break;
      }

      if (read->items.size() > 0) {
        f->instructions.insert(f->instructions.begin()+k, read);
        k++;
      }

      if (write->items.size() > 0) {
        f->instructions.insert(f->instructions.begin()+k+1, write);
        k++;
      }
    }
  }

  void spill(L2::Function * f, std::vector< std::string > spilling_table) {
    for (int k = 0; k < spilling_table.size(); k++) {
      // spill one var
      L2::spill_var(f, &spilling_table[k]);
    }
  }
}
