// by Zhiping

#include <parser.h>
#include <graph.h>
#include <spill.h>

std::string warp_item(L2::Item * i, L2::Graph * g) {
  // std::string str;

  switch (i->type) {
    case L2::ITEM::REGISTER:
          return i->name;
    case L2::ITEM::LABEL:
          return ":" + i->name;
    case L2::ITEM::NUMBER:
          return std::to_string(i->value);
    case L2::ITEM::VAR:
          return g->get_color(i->name);
    case L2::ITEM::LIBFUNC:
          return i->name;
  }
}

void output_instruction(L2::Instruction * i, std::ofstream * outputFile, L2::Graph * g, int locals) {
  *outputFile << "\t\t";

  switch (i->type) {
    case L2::INS::RETURN:
            *outputFile << "(return)";
            break;
    case L2::INS::LABEL_INS:
            *outputFile << ":" << i->items[0]->name;
            break;
    case L2::INS::MEM_START:
            *outputFile << "((mem " << warp_item(i->items[0], g) << " " << std::to_string(i->items[0]->value) << ") " << i->op << " " << warp_item(i->items[1], g) << ")";
            break;
    case L2::INS::W_START:
            if ((i->items[1]->type == L2::ITEM::REGISTER || i->items[1]->type == L2::ITEM::VAR) && i->items[1]->value != -1) {
              *outputFile << "(" << warp_item(i->items[0], g) << " " << i->op << " (mem " << warp_item(i->items[1], g) << " " << std::to_string(i->items[1]->value) << "))";
            } else {
              *outputFile << "(" << warp_item(i->items[0], g) << " " << i->op << " " << warp_item(i->items[1], g) << ")";
            }
            break;
    case L2::INS::CALL:
            *outputFile << "(call " << warp_item(i->items[0], g) << " " << std::to_string(i->items[0]->value) << ")";
            break;
    case L2::INS::GOTO:
            *outputFile << "(goto " << warp_item(i->items[0], g) << ")";
            break;
    case L2::INS::INC_DEC:
            *outputFile << "(" << warp_item(i->items[0], g) << i->op << ")";
            break;
    case L2::INS::CISC:
            *outputFile << "(" << warp_item(i->items[0], g) << " @ " << warp_item(i->items[1], g) << " " << warp_item(i->items[2], g) << " " << std::to_string(i->items[2]->value) << ")";
            break;
    case L2::INS::CMP:
            *outputFile << "(" << warp_item(i->items[0], g) << " <- " << warp_item(i->items[1], g) << " " << i->op << " " << warp_item(i->items[2], g) << ")";
            break;
    case L2::INS::CJUMP:
            *outputFile << "(cjump " << warp_item(i->items[0], g) << " " << i->op << " " << warp_item(i->items[1], g) << " " << warp_item(i->items[2], g) << " " << warp_item(i->items[3], g) << ")";
            break;
    case L2::INS::STACK:
            *outputFile << "(" << warp_item(i->items[0], g) << " " << i->op << " (mem rsp " << std::to_string(i->items[1]->value + locals * 8) << "))";
            break;
    default:
            break;
  }

  *outputFile << "\n";
}


void output_function(L2::Function * f, std::ofstream * outputFile, L2::Graph * g) {
  *outputFile << "\t(:" << f->name << "\n\t\t" << f->arguments << " " << f->locals << "\n";

  for (auto i : f->instructions) {
    output_instruction(i, outputFile, g, f->locals);
  }

  *outputFile << "\t)\n\n";
}

int main(int argc, char **argv) {
  bool verbose;


  /* Check the input */
  if( argc < 2 ) {
  std::cerr << "Usage: " << argv[ 0 ] << " SOURCE [-v]" << std::endl;
    return 1;
  }
  int32_t opt;
  while ((opt = getopt(argc, argv, "v")) != -1) {
    switch (opt) {
      case 'v':
        verbose = true;
        break;

      default:
        std::cerr << "Usage: " << argv[ 0 ] << "[-v] SOURCE" << std::endl;
        return 1;
    }
  }

  std::ofstream outputFile;
  outputFile.open("prog.L1");

  L2::Program p = L2::L2_parse_file(argv[optind]);

  outputFile << "(:" << p.entryPointLabel << "\n";

  for (auto f : p.functions) {
    std::vector< std::string > spilling_table;
    L2::Graph g;

    do {
      std::cout << f->name << ":\n";

      g = L2::Graph(f, 15);
      spilling_table = g.coloring();
      std::cout << "total color used " << 15 + spilling_table.size() << "\n";

      if (spilling_table.size() > 0) {
        L2:spill(f, spilling_table);
      }
    } while (spilling_table.size() > 0);

    output_function(f, &outputFile, &g);
  }

  outputFile << ")" << "\n";
  outputFile.close();

  return 0;
}
