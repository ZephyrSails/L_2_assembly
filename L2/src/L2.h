// by: Zhiping
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

namespace L2 {

  // const int ITEM_REGISTER = 0;
  // const int ITEM_LABEL = 1;
  // const int ITEM_NUMBER = 2;
  // const int ITEM_VAR = 3;
  enum ITEM {
    REGISTER, LABEL, NUMBER, VAR
  };

  enum INS {
    RETURN, LABEL_INS, W_START, MEM_START, CALL, GOTO, INC_DEC,
    CISC, CMP, CJUMP, STACK
  };
  // const int INS_RETURN = 0;
  // const int INS_LABEL = 1;
  // const int INS_W_START = 2;
  // const int INS_MEM_START = 10;
  // const int INS_CALL = 3;
  // const int INS_GOTO = 4;
  // const int INS_INC_DEC = 5;
  // const int INS_CISC = 6;
  // const int INS_CMP = 7;
  // const int INS_CJUMP = 8;
  // const int INS_STACK = 9;

  struct L2_item {
    std::string labelName;
  };

  struct Item {
    int type;         // defined by L1::ITEM_*TYPE*
    std::string name; // for register & label
    int value;        // for register (offset) & number
  };

  struct Instruction {
    int type;     // defined by L1::INS_*TYPE*
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
