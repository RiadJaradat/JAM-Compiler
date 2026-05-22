

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

#include "front_end.hpp"
#include "helper.hpp"
#include "machine-code.hpp"

std::ofstream bin_file("code", std::ios::binary);

mc::x86_64::ELF elf;
mc::x86_64::Header program_header;
mc::x86_64::machine_code code_bytes;
mc::x86_64::machine_code data_bytes;

std::map<std::vector<uint8_t>, size_t> program_table;
std::map<size_t, size_t> ptr_size;
std::map<size_t, size_t> Addresses;
std::map<std::string, size_t> labels;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    throw std::runtime_error("JAM: No input file");
  }

  std::ifstream scource(argv[1]);

  translator::Token_map tokens = translator::getTokens(scource);
  translator::parse(tokens);

  labels["_start"] = code_bytes.size();
  helper::__print(code_bytes, program_table, ptr_size, "Hello, World!\n");
  helper::__exit(code_bytes, 0);

  helper::patch(code_bytes, data_bytes, program_table, ptr_size, Addresses,
                program_header);
  mc::x86_64::machine_code bytes =
      helper::combin_excutable(elf, program_header, code_bytes, data_bytes);
  helper::write(bin_file, bytes);

  bin_file.close();
  return 0;
}