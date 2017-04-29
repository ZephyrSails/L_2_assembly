// by: Zhiping

#pragma once

#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <tuple>
#include <unistd.h>

namespace L2 {

  const std::vector< std::string > REGS = { "r10", "r11", "r8", "r9", "rax", "rcx", "rdi", "rdx", "rsi", "r12", "r13", "r14", "r15", "rbp", "rbx" };
  // const std::vector< std::string > sys

  enum ITEM {
    REGISTER, LABEL, NUMBER, VAR, LIBFUNC
  };

  enum INS {
    RETURN, LABEL_INS, W_START, MEM_START, CALL, GOTO, INC_DEC,
    CISC, CMP, CJUMP, STACK
  };

  struct L2_item {
    std::string labelName;
  };

  struct Item {
    int type;         // defined by L1::ITEM_*TYPE*
    std::string name; // for register & label
    int64_t value;        // for register (offset) & number
  };

  struct Instruction {
    int type;         // defined by L1::INS_*TYPE*
    std::vector<L2::Item *> items;
    std::string op;
  };

  struct Function {
    std::string name;
    int64_t arguments;
    int64_t locals;
    std::vector<L2::Instruction *> instructions;
  };

  struct Program {
    std::string entryPointLabel;
    std::vector<L2::Function *> functions;
  };
}
