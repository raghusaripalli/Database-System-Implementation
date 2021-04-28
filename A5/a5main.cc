#include <iostream>

#include "Execute.h"

using namespace std;

char* catalog_path = "catalog";
char* dbfile_dir = "bin/";
char* tpch_dir = "../tables";

int main (int argc, char* argv[]) {
  Execute execute;
  execute.run();
  return 0;
}
