// by Zhiping

#include <parser.h>
#include <graph.h>

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

  L2::Program p = L2::L2_parse_file(argv[optind]);

  for (auto f : p.functions) {
    int n = f->instructions.size();

    // std::set<std::string> IN[n];
    // std::set<std::string> OUT[n];
    L2::Graph g = L2::Graph(f);
    // L2::code_analysis(f);

  }

  return 0;
}
