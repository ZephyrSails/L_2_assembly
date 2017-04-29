// by: Zhiping

// #include <L2.h>
#include <pegtl.hh>
#include <pegtl/analyze.hh>
#include <pegtl/contrib/raw_string.hh>
#include <parser.h>

using namespace pegtl;
using namespace std;

namespace L2 {
  // meta element
  struct comment:
    pegtl::disable<
      pegtl::one< ';' >,
      pegtl::until< pegtl::eolf >
    > {};

  struct seps:
    pegtl::star<
      pegtl::sor<
        pegtl::ascii::space,
        comment
      >
    > {};

  struct aop:
    pegtl::sor<
      pegtl::string< '+', '=' >,
      pegtl::string< '-', '=' >,
      pegtl::string< '*', '=' >,
      pegtl::string< '&', '=' >
    > {};

  struct sop:
    pegtl::sor<
      pegtl::string< '<', '<', '=' >,
      pegtl::string< '>', '>', '=' >
    > {};

  struct cmp:
    pegtl::sor<
      pegtl::string< '<', '=' >,
      pegtl::string< '<'>,
      pegtl::string< '=' >
    > {};

  struct E:
    pegtl::sor<
      pegtl::one< '0' >,
      pegtl::one< '2' >,
      pegtl::one< '4' >,
      pegtl::one< '8' >
    > {};

  struct runtime_system_func:
    pegtl::sor<
    pegtl::string< 'p', 'r', 'i', 'n', 't' >,
    pegtl::string< 'a', 'l', 'l', 'o', 'c', 'a', 't', 'e' >,
    pegtl::string< 'a', 'r', 'r', 'a', 'y', '-', 'e', 'r', 'r', 'o', 'r' >
  > {};

  struct mem:
    pegtl::string < 'm', 'e', 'm' > {};

  struct inc_dec:
    pegtl::sor<
      pegtl::string < '+', '+' >,
      pegtl::string < '-', '-' >
    > {};

  struct call:
    pegtl::string < 'c', 'a', 'l', 'l' > {};

  struct left_arrow:
    pegtl::string< '<', '-' > {};

  struct plus_minus_op:
    pegtl::sor<
      pegtl::string< '+', '='>,
      pegtl::string< '-', '='>
    > {};

  // basic elements, definition.

  struct L2_var:
    pegtl::seq<
      pegtl::plus<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >
        >
      >,
      pegtl::star<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >,
          pegtl::digit
        >
      >
    > {};

  struct L2_label:
    pegtl::seq<
      pegtl::one<':'>,
      L2_var
    > {};

  struct L2_N:
    pegtl::seq<
      pegtl::opt<
        pegtl::sor<
          pegtl::one< '-' >,
          pegtl::one< '+' >
        >
      >,
      pegtl::plus<
        pegtl::digit
      >
    > {};

  struct L2_sx:
    pegtl::sor<
      pegtl::string< 'r', 'c', 'x' >,
      L2_var
    > {};

  struct L2_a:
    pegtl::sor<
      pegtl::string < 'r', 'd', 'i' >,
      pegtl::string < 'r', 's', 'i' >,
      pegtl::string < 'r', 'd', 'x' >,
      L2_sx,
      pegtl::string < 'r', '8' >,
      pegtl::string < 'r', '9' >
    > {};

  struct L2_w:
    pegtl::sor<
      L2_a,
      pegtl::string < 'r', 'a', 'x' >,
      pegtl::string < 'r', 'b', 'x' >,
      pegtl::string < 'r', 'b', 'p' >,
      pegtl::string < 'r', '1', '0' >,
      pegtl::string < 'r', '1', '1' >,
      pegtl::string < 'r', '1', '2' >,
      pegtl::string < 'r', '1', '3' >,
      pegtl::string < 'r', '1', '4' >,
      pegtl::string < 'r', '1', '5' >
    > {};

  struct L2_x:
    pegtl::sor<
      L2_w,
      pegtl::string < 'r', 's', 'p' >
    > {};

  struct L2_s:
    pegtl::sor<
      L2_x,
      L2_N,
      L2_label
    > {};

  struct L2_M:
    L2_N {};

  struct L2_t:
    pegtl::sor<
      L2_x,
      L2_N
    > {};

  struct L2_u:
    pegtl::sor<
      L2_w,
      L2_label
    > {};

  // special elements

  struct function_name:
    L2_label {};

  struct argument_number:
    L2_N {};

  struct local_number:
    L2_N {};

  struct prog_label:
    L2_label {};

  struct L2_label_rule:
    L2_label {};

  // element decleration

  struct var:
    L2_var {};
  struct label:
    L2_label {};
  struct N:
    L2_N {};
  struct sx:
    L2_sx {};
  struct a:
    L2_a {};
  struct w:
    L2_w {};
  struct x:
    L2_x {};
  struct s:
    L2_s {};
  struct M:
    L2_M {};
  struct t:
    L2_t {};
  struct u:
    L2_u {};

  // instructure

  struct mem_x_M:
    pegtl::seq<
      pegtl::one< '(' >,
      seps,
      mem,
      seps,
      x,
      seps,
      M,
      seps,
      pegtl::one< ')' >
    > {};

  struct stack_arg_sign:
    pegtl::string<'s', 't', 'a', 'c', 'k', '-', 'a', 'r', 'g'> {};

  struct stack_arg:
    pegtl::seq<
      pegtl::one<'('>,
      seps,
      stack_arg_sign,
      seps,
      M,
      seps,
      pegtl::one<')'>
    > {};

  struct ins_w_start:
    pegtl::seq<
      w,
      seps,
      pegtl::sor<
        pegtl::seq<
          left_arrow,
          seps,
          pegtl::sor<
            pegtl::seq<
              s, // potential bug
              pegtl::star< pegtl::seq< seps, cmp, seps, t > >
            >,
            mem_x_M,
            stack_arg
          >
        >, // potential bug
        pegtl::seq<
          aop,
          seps,
          pegtl::sor< t, mem_x_M >
        >,
        pegtl::seq< sop, seps, pegtl::sor< sx, N > >,
        inc_dec,
        pegtl::seq< pegtl::one< '@' >, seps, w, seps, w, seps, E >
      >
    > {};

  struct ins_mem_start:
    pegtl::seq<
      mem_x_M,
      seps,
      pegtl::sor<
        pegtl::seq< left_arrow, seps, s >,
        pegtl::seq< plus_minus_op, seps, t >
      >
    > {};

  struct ins_cjump:
    pegtl::seq<
      seps,
      pegtl::string< 'c', 'j', 'u', 'm', 'p' >,
      seps,
      t,
      seps,
      cmp,
      seps,
      t,
      seps,
      label,
      seps,
      label
    > {};

  struct ins_label:
    label {};

  struct ins_goto:
    pegtl::seq< pegtl::string < 'g', 'o', 't', 'o' >, seps, label > {};

  struct ins_return:
    pegtl::seq< pegtl::string < 'r', 'e', 't', 'u', 'r', 'n' > > {};

  struct ins_call_func:
    pegtl::seq<
      call,
      seps,
      pegtl::sor< u, runtime_system_func >,
      seps,
      N
    > {};

  struct L2_instruction:
    pegtl::sor<
      pegtl::seq<
        pegtl::one<'('>,
        seps,
        pegtl::sor<
          ins_goto,
          ins_return,
          ins_call_func
        >,
        seps,
        pegtl::one<')'>
      >,
      pegtl::seq<
        pegtl::one<'('>,
        pegtl::sor<
          ins_mem_start,
          ins_cjump,
          ins_w_start
        >,
        pegtl::one<')'>
      >,
      ins_label
    > {};

  struct L2_function_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      function_name,
      seps,
      argument_number,
      seps,
      local_number,
      seps,
      pegtl::star<
        L2_instruction,
        seps
      >,
      seps,
      pegtl::one< ')' >,
      seps
    > {};

  struct L2_functions_rule:
    pegtl::seq<
      seps,
      pegtl::plus< L2_function_rule >
    > {};

  struct entry_point_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      seps,
      prog_label,
      seps,
      L2_functions_rule,
      seps,
      pegtl::one< ')' >,
      seps
    > {};

  struct L2_grammer:
    pegtl::must<entry_point_rule> {};

  /*
   * Helpers
   */
  Item * new_item(std::string str) {
    Item *item = new Item();
    if (str[0] == ':') { // label
      item->type = L2::ITEM::LABEL;
      str.erase(0, 1);  // remove ":"
      item->name = str;
    } else if (std::find(L2::REGS.begin(), L2::REGS.end(), str) != L2::REGS.end()) { // register
    // } else if (str[0] == 'r') { // register
      item->type = L2::ITEM::REGISTER;
      item->name = str;
      item->value = -1;
    } else if (str == "print" || str == "allocate" || str == "array-error") {
      item->type = L2::ITEM::LIBFUNC;
      item->name = str;
      item->value = -1;
    } else {
      try { // number
        item->value = std::stoll(str);
        item->type = L2::ITEM::NUMBER;
      } catch (const std::exception& e) { // var
        item->type = L2::ITEM::VAR;
        //  str.erase(0, 1);
        item->name = str;
        item->value = -1;
      }
    }
    return item;
  }
  //
  Item * new_item2(std::string reg, std::string offset) {
    Item *item = new_item(reg);
    //  item = ;
    item->value = std::stoll(offset);
    return item;
  }

  /*
   * Actions attached to grammar rules.
   */
  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};


  template<> struct action < prog_label > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      if (p.entryPointLabel.empty()) {
        std::string token = in.string();
        token.erase(0, 1);

        p.entryPointLabel = token;
      }
    }
  };

  template<> struct action < function_name > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      L2::Function *newF = new L2::Function();

      std::string token = in.string();
      token.erase(0, 1);

      newF->name = token;
      p.functions.push_back(newF);
      // f->name = token;
      v.clear();
    }
  };

  template<> struct action < L2_label_rule > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      L2_item i;
      i.labelName = in.string();
      // parsed_registers.push_back(i);
      v.clear();
    }
  };

  template<> struct action < argument_number > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      L2::Function *currentF = p.functions.back();
      currentF->arguments = std::stoll(in.string());
    }
  };

  template<> struct action < local_number > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      L2::Function *currentF = p.functions.back();
      currentF->locals = std::stoll(in.string());
    }
  };

  template<> struct action < ins_w_start > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      L2::Function *currentF = p.functions.back();
      L2::Instruction *newIns = new L2::Instruction();

      if (v.size() == 3) { // no mem, two op
        newIns->type = L2::INS::W_START;
        // cout << "tinkering no mem assign: " << v.at(0) << " " << v.at(1) << " " << v.at(2) << endl;
        newIns->items.push_back(L2::new_item(v.at(0)));
        newIns->items.push_back(L2::new_item(v.at(2)));
        newIns->op = v.at(1);

      } else if (v.size() == 2) { // INS_INC_DEC
        newIns->type = L2::INS::INC_DEC;

        // cout << "tinkering ins_inc_dec: " << v.at(0) << v.at(1) << endl;
        newIns->op = v.at(1);
        newIns->items.push_back(new_item(v.at(0)));
      } else if (v.size() == 4) {
        if (v.at(2) == "stack-arg") { // stack-arg
          // std::cout << "hello stack\n";
          newIns->type = L2::INS::STACK;
          newIns->op = v[1];

          newIns->items.push_back(new_item(v.at(0)));
          newIns->items.push_back(new_item(v.at(3)));
        } else { // CISC
          newIns->type = L2::INS::CISC;

          // cout << "tinkering CISC: " << v.at(0) << " @ " << v.at(1) << " " << v.at(2) << " " << v.at(3) << endl;
          newIns->items.push_back(new_item(v.at(0)));
          newIns->items.push_back(new_item(v.at(1)));
          newIns->items.push_back(new_item2(v.at(2), v.at(3)));
        }
      } else if (v.at(2) == "mem") { // right mem two op
        newIns->type = L2::INS::W_START;
        // cout << "tinkering right mem (" << v.at(0) << " " << v.at(1) << " (" << v.at(3) << " " << v.at(4) << ")" << endl;
        newIns->items.push_back(L2::new_item(v.at(0)));
        newIns->items.push_back(L2::new_item2(v.at(3), v.at(4)));
        newIns->op = v.at(1);
      } else { // cmp, 5 cmp
        newIns->type = L2::INS::CMP;

        // cout << "tinkering cmp " << v.at(0) << " " << v.at(1) << " " << v.at(2) << " " << v.at(3) << " " << v.at(4)<< endl;
        newIns->items.push_back(L2::new_item(v.at(0)));
        newIns->items.push_back(L2::new_item(v.at(2)));
        newIns->items.push_back(L2::new_item(v.at(4)));
        newIns->op = v.at(3);
      }
      currentF->instructions.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_mem_start > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      L2::Function *currentF = p.functions.back();
      L2::Instruction *newIns = new L2::Instruction();
      newIns->type = L2::INS::MEM_START;

      newIns->items.push_back(L2::new_item2(v.at(1), v.at(2)));
      newIns->items.push_back(L2::new_item(v.at(4)));
      newIns->op = v.at(3);

      currentF->instructions.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_cjump > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      L2::Function *currentF = p.functions.back();
      L2::Instruction *newIns = new L2::Instruction();
      newIns->type = L2::INS::CJUMP;

      // cout << "tinkering cjump " << v.at(0) << " " << v.at(1) << " " << v.at(2) << " " << v.at(3) << " " << v.at(4)<< endl;
      newIns->items.push_back(L2::new_item(v.at(0)));
      newIns->op = v.at(1);
      newIns->items.push_back(L2::new_item(v.at(2)));
      newIns->items.push_back(L2::new_item(v.at(3)));
      newIns->items.push_back(L2::new_item(v.at(4)));

      currentF->instructions.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_call_func > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {

      L2::Function *currentF = p.functions.back();
      L2::Instruction *newIns = new L2::Instruction();
      newIns->type = L2::INS::CALL;

      // cout << "tinkering " << v.at(0) << " ?? " << v.at(1) << " - " << v.at(2) << endl;

      newIns->items.push_back(L2::new_item2(v.at(1), v.at(2)));
      currentF->instructions.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_return > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      L2::Function *currentF = p.functions.back();
      L2::Instruction *newIns = new L2::Instruction();

      // cout << "tinkering call return" << endl;

      newIns->type = L2::INS::RETURN;

      currentF->instructions.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_label > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      L2::Function *currentF = p.functions.back();
      L2::Instruction *newIns = new L2::Instruction();
      newIns->type = L2::INS::LABEL_INS;

      L2::Item *newItem = new L2::Item();
      newIns->items.push_back(new_item(in.string()));

      // cout << "tinkering label: " << in.string() << endl;

      currentF->instructions.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_goto > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      L2::Function *currentF = p.functions.back();
      L2::Instruction *newIns = new L2::Instruction();
      newIns->type = L2::INS::GOTO;

      L2::Item *newItem = new L2::Item();
      newIns->items.push_back(new_item(v.at(0)));

      // cout << "tinkering ins_goto: " << v.at(0) << endl;

      currentF->instructions.push_back(newIns);
      v.clear();
    }
  };

  //
  // Actions to collect string from rules, should be a better way.
  //
  template<> struct action < stack_arg_sign > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < inc_dec > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action runtime_system_func " << in.string() << endl;
    }
  };

  template<> struct action < runtime_system_func > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action runtime_system_func " << in.string() << endl;
    }
  };

  template<> struct action < var > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action call " << in.string() << endl;
    }
  };

  template<> struct action < call > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action call " << in.string() << endl;
    }
  };

  template<> struct action < cmp > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action cmp " << in.string() << endl;
    }
  };

  template<> struct action < u > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action u " << in.string() << endl;
    }
  };

  template<> struct action < N > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action N " << in.string() << endl;
    }
  };

  template<> struct action < plus_minus_op > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action plus_minus_op " << in.string() << endl;
    }
  };

  template<> struct action < sx > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action rcx " << in.string() << endl;
    }
  };

  template<> struct action < mem > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action mem " << in.string() << endl;
    }
  };

  template<> struct action < t > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action t " << in.string() << endl;
    }
  };

  template<> struct action < w > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action w " << in.string() << endl;
    }
  };

  template<> struct action < left_arrow > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action left_arrow " << in.string() << endl;
    }
  };

  template<> struct action < s > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action s " << in.string() << endl;
    }
  };

  template<> struct action < x > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action x " << in.string() << endl;
    }
  };

  template<> struct action < aop > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action aop " << in.string() << endl;
    }
  };

  template<> struct action < sop > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action sop " << in.string() << endl;
    }
  };

  template<> struct action < M > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action M " << in.string() << endl;
    }
  };

  template<> struct action < E > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action E " << in.string() << endl;
    }
  };

  template<> struct action < label > {
    static void apply( const pegtl::input & in, L2::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
      // cout << "tinkering action label " << in.string() << endl;
    }
  };

  /*
   * Data structures required to parse
   */
  std::vector< L2_item > parsed_registers;

  Program L2_parse_func_file (char *fileName) {
    /*
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< L2::L2_function_rule >();
    /*
     * Parse.
     */
    L2::Program p;
    // L2::Instruction ti; // temp instruction
    std::vector<std::string> v;
    pegtl::file_parser(fileName).parse< L2::L2_function_rule, L2::action > (p, v);

    return p;
  }

  Program L2_parse_file (char *fileName) {
    /*
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< L2::L2_grammer >();
    /*
     * Parse.
     */
    L2::Program p;
    // L2::Instruction ti; // temp instruction
    std::vector<std::string> v;
    pegtl::file_parser(fileName).parse< L2::L2_grammer, L2::action > (p, v);

    return p;
  }
}
