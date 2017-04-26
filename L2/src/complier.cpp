// by Zhiping

#include <parser.h>
#include <graph.h>
#include <spill.h>

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
    std::vector< std::string > spilling_table;
    do {
      std::cout << f->name << ":\n";

      L2::Graph g = L2::Graph(f, 15);
      spilling_table = g.coloring();
      std::cout << "total color used " << 15 + spilling_table.size() << "\n";

      if (spilling_table.size() > 0) {
        L2:spill(f, spilling_table);
      }
    } while (spilling_table.size() > 0);
  }

  return 0;
}
