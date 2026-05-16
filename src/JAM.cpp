

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include "machine-code.hpp"

std::vector<uint8_t> bytes;
std::ofstream bin_file("code.bin", std::ios::binary);

void write(std::ofstream &FILE, const std::vector<uint8_t> &BYTES) {
  FILE.write(reinterpret_cast<const char *>(BYTES.data()), BYTES.size());
}

mc::ELF elf; // stick with basic values
mc::Header program_header;
mc::machine_code code_bytes;

int main(int argc, char *argv[]) {

  code_bytes = {mc::MOV_EAX, 60,   0x00, 0x00, 0x00,          mc::MOV_EDI,
                0x00,        0x00, 0x00, 0x00, mc::SYSCALL_1, mc::SYSCALL_2};

  program_header.p_filesz = mc::num_to_bytes(mc::linux_header_size + code_bytes.size(), 8);
  program_header.p_memsz = mc::num_to_bytes(mc::linux_header_size + code_bytes.size(), 8);
  
  std::vector<uint8_t> elf_bytes = elf.to_bytes();
  std::vector<uint8_t> header_bytes = program_header.to_bytes();

  bytes.insert(bytes.end(), elf_bytes.begin(), elf_bytes.end());
  bytes.insert(bytes.end(), header_bytes.begin(), header_bytes.end());
  bytes.insert(bytes.end(), code_bytes.begin(), code_bytes.end());
  
  write(bin_file, bytes);

  bin_file.close();
  return 0;
}