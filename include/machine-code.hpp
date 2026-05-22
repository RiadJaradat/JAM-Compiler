#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace mc {
namespace x86_64 {
inline static const std::vector<uint8_t> ELF_hardcoded = {
    0x7F, 'E',  'L',  'F',  0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,

    0x02, 0x00, 0x3E, 0x00, 0x01, 0x00, 0x00, 0x00,

    0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00,

    0x40, 0x00, 0x38, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

inline static const uint8_t MOV_EAX = 0xB8;
inline static const uint8_t MOV_EDI = 0xBF;
inline static const uint8_t MOV_ESI = 0xBE;
inline static const uint8_t MOV_EDX = 0xBA;
inline static const uint8_t JMP_ADD = 0xE9;
inline static const uint8_t SYSCALL_1 = 0x0F;
inline static const uint8_t SYSCALL_2 = 0x05;

inline static const int linux_header_size = 120;

template <typename T>
std::vector<uint8_t> num_to_bytes(T value, size_t num_bytes) {
  if (num_bytes > sizeof(T)) {
    throw std::invalid_argument(
        "Requested byte size exceeds the type capability.");
  }

  std::vector<uint8_t> bytes;
  bytes.reserve(num_bytes);

  for (size_t i = 0; i < num_bytes; ++i) {
    // Shift right by 8 * i bits and mask out the lowest byte
    bytes.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
  }

  return bytes;
}

template <typename T> T bytes_to_num(const std::vector<uint8_t> &bytes) {
  T value = 0;

  for (size_t i = 0; i < bytes.size(); i++) {
    value |= static_cast<T>(bytes[i]) << (i * 8);
  }

  return value;
}

inline static size_t memo_place(size_t offset, int start_size = 0x400000) {
  return start_size + mc::x86_64::linux_header_size + offset;
}

typedef std::vector<uint8_t> machine_code;
class ELF {
public:
  std::vector<uint8_t> ELF_signeture; // 7F 'E' 'L' 'F'
  uint8_t ELF_classBIT;               // 02 64-bit
  uint8_t endian_type;                // 01 little endian
  uint8_t version;                    // 01
  uint8_t ABI;                        // 00 SYSV
  std::vector<uint8_t> padding;       // 8 bytes zeros
  std::vector<uint8_t> e_type;        // 2 bytes
  std::vector<uint8_t> e_machine;     // 2 bytes
  std::vector<uint8_t> e_version;     // 4 bytes
  std::vector<uint8_t> e_entry;       // 8 bytes
  std::vector<uint8_t> e_phoff;       // 8 bytes
  std::vector<uint8_t> e_shoff;       // 8 bytes
  std::vector<uint8_t> e_flags;       // 4 bytes
  std::vector<uint8_t> e_ehsize;      // 2 bytes
  std::vector<uint8_t> e_phentsize;   // 2 bytes
  std::vector<uint8_t> e_phnum;       // 2 bytes
  std::vector<uint8_t> e_shentsize;   // 2 bytes
  std::vector<uint8_t> e_shnum;       // 2 bytes
  std::vector<uint8_t> e_shstrndx;    // 2 bytes

  ELF() {
    ELF_signeture = {0x7F, 'E', 'L', 'F'};
    ELF_classBIT = 0x02;
    endian_type = 0x01;
    version = 0x01;
    ABI = 0x00;
    padding = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    // 2. File and Architecture Type
    e_type = {0x02, 0x00};                // 2 bytes: ET_EXEC (Executable)
    e_machine = {0x3E, 0x00};             // 2 bytes: EM_X86_64 (AMD64)
    e_version = {0x01, 0x00, 0x00, 0x00}; // 4 bytes: ELF Version 1

    // 3. Addresses and Offsets (Little-Endian Order)
    // Memory address where the program starts (e.g., 0x400078)
    e_entry = {0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Program header offset (0x40 / 64 bytes - right after this header)
    e_phoff = {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Section header offset (0x00000000 - none used for execution)
    e_shoff = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // 4. Sizes and Flag Metadata
    e_flags = {0x00, 0x00, 0x00, 0x00}; // 4 bytes: No flags for x86_64
    e_ehsize = {0x40, 0x00};            // 2 bytes: ELF header size (64 bytes)
    e_phentsize = {0x38, 0x00}; // 2 bytes: Program header size (56 bytes)
    e_phnum = {0x01, 0x00};     // 2 bytes: 1 Program Header entry
    e_shentsize = {0x40, 0x00}; // 2 bytes: Section header size (64 bytes)
    e_shnum = {0x00, 0x00};     // 2 bytes: 0 Section Header entries
    e_shstrndx = {0x00, 0x00};  // 2 bytes: No section name string table
  }

  std::vector<uint8_t> to_bytes() {
    std::vector<uint8_t> returned;

    returned.insert(returned.end(), ELF_signeture.begin(), ELF_signeture.end());
    returned.push_back(ELF_classBIT);
    returned.push_back(endian_type);
    returned.push_back(version);
    returned.push_back(ABI);
    returned.insert(returned.end(), padding.begin(), padding.end());
    returned.insert(returned.end(), e_type.begin(), e_type.end());
    returned.insert(returned.end(), e_machine.begin(), e_machine.end());
    returned.insert(returned.end(), e_version.begin(), e_version.end());
    returned.insert(returned.end(), e_entry.begin(), e_entry.end());
    returned.insert(returned.end(), e_phoff.begin(), e_phoff.end());
    returned.insert(returned.end(), e_shoff.begin(), e_shoff.end());
    returned.insert(returned.end(), e_flags.begin(), e_flags.end());
    returned.insert(returned.end(), e_ehsize.begin(), e_ehsize.end());
    returned.insert(returned.end(), e_phentsize.begin(), e_phentsize.end());
    returned.insert(returned.end(), e_phnum.begin(), e_phnum.end());
    returned.insert(returned.end(), e_shentsize.begin(), e_shentsize.end());
    returned.insert(returned.end(), e_shnum.begin(), e_shnum.end());
    returned.insert(returned.end(), e_shstrndx.begin(), e_shstrndx.end());

    return returned;
  }
};
class Header {
public:
  std::vector<uint8_t> p_type;  // Value: {0x01, 0x00, 0x00, 0x00}
  std::vector<uint8_t> p_flags; // Value: {0x05, 0x00, 0x00, 0x00}
  std::vector<uint8_t>
      p_offset; // Value: {0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
  std::vector<uint8_t> p_vaddr; // Value: {0x78 -> p_offset, 0x00, 0x40, 0x00,
                                // 0x00, 0x00, 0x00, 0x00}
  std::vector<uint8_t>
      p_paddr; // Value: All zeros {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
               // 0x00} * ignored in modern linux
  std::vector<uint8_t>
      p_filesz; // Value: {0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} ->
                // how long the machine code instructions are
  std::vector<uint8_t>
      p_memsz; // Value: same as p_filesz unless for .bss, make it more
  std::vector<uint8_t> p_align; // Value: {0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
                                // 0x00, 0x00} -> usually a power of 2

  Header() {
    p_type = {0x01, 0x00, 0x00, 0x00};  // PT_LOAD
    p_flags = {0x05, 0x00, 0x00, 0x00}; // PF_R | PF_X (Read + Execute)

    // Start mapping from the very beginning of the file (byte 0)
    p_offset = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Map it to the base image address
    p_vaddr = {0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00}; // 0x400000
    p_paddr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // The sizes must now include the headers + your machine code!
    // Total size = 64 (ELF) + 56 (PH) + 7 (Code) = 127 bytes (0x7F)
    p_filesz = {0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    p_memsz = {0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Standard 4KB page alignment
    p_align = {0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 0x1000
  }

  std::vector<uint8_t> to_bytes() {
    std::vector<uint8_t> returned;

    returned.insert(returned.end(), p_type.begin(), p_type.end());
    returned.insert(returned.end(), p_flags.begin(), p_flags.end());
    returned.insert(returned.end(), p_offset.begin(), p_offset.end());
    returned.insert(returned.end(), p_vaddr.begin(), p_vaddr.end());
    returned.insert(returned.end(), p_paddr.begin(), p_paddr.end());
    returned.insert(returned.end(), p_filesz.begin(), p_filesz.end());
    returned.insert(returned.end(), p_memsz.begin(), p_memsz.end());
    returned.insert(returned.end(), p_align.begin(), p_align.end());

    return returned;
  }
};
} // namespace x86_64
} // namespace mc