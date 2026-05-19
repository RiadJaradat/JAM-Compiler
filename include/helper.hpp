#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <map>
#include <vector>

#include "machine-code.hpp"

namespace helper {

// patches program pointers and sizes
inline void patch(mc::machine_code &bytes, mc::machine_code &data_bytes,
                  std::map<mc::machine_code, size_t> &program_table,
                  std::map<size_t, size_t> &pointer_and_sizes,
                  std::map<size_t, size_t> addresses,
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

  for (auto &[location, target] : addresses) {
    int32_t rel = target - (location + 4);

    *reinterpret_cast<int32_t *>(&bytes[location]) = rel;
  }

  int file_total_size =
      mc::linux_header_size + bytes.size() + data_bytes.size();

  program_header.p_filesz = mc::num_to_bytes<uint64_t>(file_total_size, 8);
  program_header.p_memsz = mc::num_to_bytes<uint64_t>(file_total_size, 8);
}

inline mc::machine_code mov(uint8_t mov_register, int value) {
  mc::machine_code res;
  res.push_back(mov_register);
  mc::machine_code number = mc::num_to_bytes(value, 4);
  res.insert(res.end(), number.begin(), number.end());
  return res;
}

inline void jmp(mc::machine_code &bytes, size_t address,
                std::map<size_t, size_t> &addresses) {
  bytes.push_back(mc::JMP_ADD);
  addresses[bytes.size()] = address;
  bytes.insert(bytes.end(), {0x00, 0x00, 0x00, 0x00});
}

inline mc::machine_code string_to_bytes(std::string value) {
  mc::machine_code ret;

  for (char c : value)
    ret.push_back(c);

  return ret;
}

inline void __print(mc::machine_code &bytes,
                    std::map<mc::machine_code, size_t> &program_table,
                    std::map<size_t, size_t> &pointer_and_sizes,
                    std::string value) {
  mc::machine_code instruction;

  instruction = helper::mov(mc::MOV_EAX, 1);

  bytes.insert(bytes.end(), instruction.begin(), instruction.end());

  instruction = helper::mov(mc::MOV_EDI, 1);

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

inline void __exit(mc::machine_code &bytes, int exit_code) {
  mc::machine_code instruction;

  instruction = mov(mc::MOV_EAX, 60);

  bytes.insert(bytes.end(), instruction.begin(), instruction.end());

  instruction = mov(mc::MOV_EDI, exit_code);

  bytes.insert(bytes.end(), instruction.begin(), instruction.end());

  bytes.insert(bytes.end(), {mc::SYSCALL_1, mc::SYSCALL_2});
}

inline mc::machine_code combin_excutable(mc::ELF &elf,
                                         mc::Header &program_header,
                                         mc::machine_code code_bytes,
                                         mc::machine_code data_bytes) {
  mc::machine_code bytes;

  mc::machine_code elf_bytes = elf.to_bytes();
  mc::machine_code header_bytes = program_header.to_bytes();

  bytes.insert(bytes.end(), elf_bytes.begin(), elf_bytes.end());
  bytes.insert(bytes.end(), header_bytes.begin(), header_bytes.end());
  bytes.insert(bytes.end(), code_bytes.begin(), code_bytes.end());
  bytes.insert(bytes.end(), data_bytes.begin(), data_bytes.end());

  return bytes;
}

inline void write(std::ofstream &FILE, const mc::machine_code &BYTES) {
  FILE.write(reinterpret_cast<const char *>(BYTES.data()), BYTES.size());
}

} // namespace helper