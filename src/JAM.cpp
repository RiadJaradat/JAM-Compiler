

#include "generall_emmiter.hpp"
#include "exception.hpp"

std::ofstream file;

int main(int argc, char *argv[]) {
  if (argc < 2)
    throw CompilerErr("No input File");


  file.open(argv[1]);
  if (!file)
    throw CompilerErr("File does not exists");

  Emmiter emmiter;

  emmiter.genLabel("_start");
  emmiter.print("hello, world!");
  emmiter.end(0);
  emmiter.genExcutable(file, argv[1],CLOSE);

  return 0;

}