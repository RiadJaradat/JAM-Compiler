

#include <cstddef>
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
mc::machine_code data_bytes;

int file_total_size;

int main(int argc, char *argv[]) {

  code_bytes.insert(code_bytes.end(), {mc::MOV_EAX, 0x01, 0x00, 0x00, 0x00});
  code_bytes.insert(code_bytes.end(), {mc::MOV_EDI, 0x01, 0x00, 0x00, 0x00});
  code_bytes.push_back(mc::MOV_ESI);
  size_t i1 = code_bytes.size();
  code_bytes.insert(code_bytes.end(), {0x00, 0x00, 0x00, 0x00});
  code_bytes.insert(code_bytes.end(), {mc::MOV_EDX, 0xA4, 0x00, 0x00, 0x00});
  code_bytes.insert(code_bytes.end(), {mc::SYSCALL_1, mc::SYSCALL_2});

  code_bytes.insert(code_bytes.end(), {mc::MOV_EAX, 60, 0x00, 0x00, 0x00});
  code_bytes.insert(code_bytes.end(), {mc::MOV_EDI, 0x00, 0x00, 0x00, 0x00});
  code_bytes.insert(code_bytes.end(), {mc::SYSCALL_1, mc::SYSCALL_2});

  data_bytes.insert(data_bytes.end(), {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n'});

  *reinterpret_cast<uint32_t*>(&code_bytes[i1]) = 0x400000 + mc::linux_header_size + code_bytes.size();

  file_total_size = mc::linux_header_size + code_bytes.size() + data_bytes.size();

  program_header.p_filesz = mc::num_to_bytes<uint64_t>(file_total_size, 8);
  program_header.p_memsz = mc::num_to_bytes<uint64_t>(file_total_size, 8);
  
  std::vector<uint8_t> elf_bytes = elf.to_bytes();
  std::vector<uint8_t> header_bytes = program_header.to_bytes();

  bytes.insert(bytes.end(), elf_bytes.begin(), elf_bytes.end());
  bytes.insert(bytes.end(), header_bytes.begin(), header_bytes.end());
  bytes.insert(bytes.end(), code_bytes.begin(), code_bytes.end());
  bytes.insert(bytes.end(), data_bytes.begin(), data_bytes.end());
  
  write(bin_file, bytes);

  bin_file.close();
  return 0;
}