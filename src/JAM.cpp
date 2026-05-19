

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "helper.hpp"
#include "machine-code.hpp"

std::ofstream bin_file("code", std::ios::binary);

mc::ELF elf;
mc::Header program_header;
mc::machine_code code_bytes;
mc::machine_code data_bytes;

std::map<std::vector<uint8_t>, size_t> program_table;
std::map<size_t, size_t> ptr_size;
std::map<size_t, size_t> Addresses;
std::map<std::string, size_t> labels;

int main(int argc, char *argv[]) {
  labels["start"] = code_bytes.size();
  helper::__print(code_bytes, program_table, ptr_size, "Hello, World!\n");
  labels["print Hi"] = code_bytes.size();
  helper::__print(code_bytes, program_table, ptr_size, "Hi\n");
  helper::__print(code_bytes, program_table, ptr_size, "Made with care\n");
  helper::jmp(code_bytes, labels["print Hi"], Addresses);
  helper::__exit(code_bytes, 0);
  helper::patch(code_bytes, data_bytes, program_table, ptr_size, Addresses,
                program_header);
  mc::machine_code bytes =
      helper::combin_excutable(elf, program_header, code_bytes, data_bytes);
  helper::write(bin_file, bytes);

  bin_file.close();
  return 0;
}