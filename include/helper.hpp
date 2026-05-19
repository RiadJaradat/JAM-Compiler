#pragma once

#include <cstdint>
#include <fstream>
#include <map>
#include <vector>

#include "machine-code.hpp"

namespace helper {

// patches program pointers and sizes
inline void patch(std::vector<uint8_t> &bytes, std::vector<uint8_t> &data_bytes,
                  std::map<std::vector<uint8_t>, size_t> &program_table,
                  std::map<size_t, size_t> &pointer_and_sizes,
                  mc::Header &program_header) {

  for (auto &[v, s] : program_table)
    data_bytes.insert(data_bytes.end(), v.begin(), v.end());

  int c1 = 0;
  size_t data_offset = 0;
  for (auto &[ptr, size] : pointer_and_sizes) {
    *reinterpret_cast<uint32_t *>(&bytes[ptr]) =
        mc::memo_place(bytes.size() + data_offset);
    *reinterpret_cast<uint32_t *>(&bytes[size]) =
        std::next(program_table.begin(), c1)->second;

    data_offset += std::next(program_table.begin(), c1)->second;
    c1++;
  }

  int file_total_size =
      mc::linux_header_size + bytes.size() + data_bytes.size();

  program_header.p_filesz = mc::num_to_bytes<uint64_t>(file_total_size, 8);
  program_header.p_memsz = mc::num_to_bytes<uint64_t>(file_total_size, 8);
}

inline std::vector<uint8_t> instruction_to_bytes(uint8_t mov_register,
                                                 int value) {
  std::vector<uint8_t> res;
  res.push_back(mov_register);
  std::vector<uint8_t> number = mc::num_to_bytes(value, 4);
  res.insert(res.end(), number.begin(), number.end());
  return res;
}

inline std::vector<uint8_t> string_to_bytes(std::string value) {
  std::vector<uint8_t> ret;

  for (char c : value)
    ret.push_back(c);

  return ret;
}

inline void __print(std::vector<uint8_t> &bytes,
                    std::map<std::vector<uint8_t>, size_t> &program_table,
                    std::map<size_t, size_t> &pointer_and_sizes,
                    std::string value) {
  std::vector<uint8_t> instruction;

  instruction = helper::instruction_to_bytes(mc::MOV_EAX, 1);

  bytes.insert(bytes.end(), instruction.begin(), instruction.end());

  instruction = helper::instruction_to_bytes(mc::MOV_EDI, 1);

  bytes.insert(bytes.end(), instruction.begin(), instruction.end());

  bytes.push_back(mc::MOV_ESI);
  size_t ptr = bytes.size();
  bytes.insert(bytes.end(), {0x00, 0x00, 0x00, 0x00});
  bytes.push_back(mc::MOV_EDX);
  pointer_and_sizes[ptr] = bytes.size();
  bytes.insert(bytes.end(), {0x00, 0x00, 0x00, 0x00});
  bytes.insert(bytes.end(), {mc::SYSCALL_1, mc::SYSCALL_2});

  program_table[helper::string_to_bytes(value)] = value.size();
}

inline void __exit(std::vector<uint8_t> &bytes, int exit_code) {
  std::vector<uint8_t> instruction;

  instruction = instruction_to_bytes(mc::MOV_EAX, 60);

  bytes.insert(bytes.end(), instruction.begin(), instruction.end());

  instruction = instruction_to_bytes(mc::MOV_EDI, exit_code);

  bytes.insert(bytes.end(), instruction.begin(), instruction.end());

  bytes.insert(bytes.end(), {mc::SYSCALL_1, mc::SYSCALL_2});
}

inline std::vector<uint8_t> combin_excutable(mc::ELF &elf,
                                             mc::Header &program_header,
                                             mc::machine_code code_bytes,
                                             mc::machine_code data_bytes) {
  std::vector<uint8_t> bytes;

  std::vector<uint8_t> elf_bytes = elf.to_bytes();
  std::vector<uint8_t> header_bytes = program_header.to_bytes();

  bytes.insert(bytes.end(), elf_bytes.begin(), elf_bytes.end());
  bytes.insert(bytes.end(), header_bytes.begin(), header_bytes.end());
  bytes.insert(bytes.end(), code_bytes.begin(), code_bytes.end());
  bytes.insert(bytes.end(), data_bytes.begin(), data_bytes.end());

  return bytes;
}

inline void write(std::ofstream &FILE, const std::vector<uint8_t> &BYTES) {
  FILE.write(reinterpret_cast<const char *>(BYTES.data()), BYTES.size());
}

} // namespace helper