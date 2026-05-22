#include <cstdint>
#include <fstream>
#include <vector>

// making a simple byte writter

int end_of_text;
int after_elf;
int total_size;

namespace inst {
inline static const uint8_t MOV_EAX = 0xB8;
inline static const uint8_t MOV_EDI = 0xBF;
inline static const uint8_t SYSCALL_1 = 0x0F;
inline static const uint8_t SYSCALL_2 = 0x05;
} // namespace inst

void move() {}

void write_bytes(const std::vector<uint8_t> &bytes, std::ofstream &file) {
  file.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());
}

void gen_ELF(std::vector<uint8_t> &bytes) {
  bytes.insert(bytes.end(), {0x7F, 'E',  'L',  'F',  0x02, 0x01, 0x01, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                             0x02, 0x00, 0x3E, 0x00, 0x01, 0x00, 0x00, 0x00,

                             0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,

                             0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                             0x00, 0x00, 0x00, 0x00,

                             0x40, 0x00, 0x38, 0x00, 0x01, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00});

  after_elf = bytes.size();
}

void gen_header(std::vector<uint8_t> &bytes,
                const std::vector<uint8_t> &file_size) {
  // 1. Type (PT_LOAD) and Flags (RX)
  bytes.insert(bytes.end(), {0x01, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00});

  // 2. p_offset (8 bytes) - Start of file
  bytes.insert(bytes.end(), {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

  // 3. p_vaddr (8 bytes) - 0x400000
  bytes.insert(bytes.end(), {0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00});

  // 4. p_paddr (8 bytes) - 0x400000
  bytes.insert(bytes.end(), {0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00});

  // 5. p_filesz (8 bytes) - THIS IS YOUR file_size vector
  bytes.insert(bytes.end(), file_size.begin(), file_size.end());

  // 6. p_memsz (8 bytes) - Usually same as filesz for simple programs
  bytes.insert(bytes.end(), file_size.begin(), file_size.end());

  // 7. p_align (8 bytes) - 0x1000 (4KB)
  bytes.insert(bytes.end(), {0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
}

std::vector<uint8_t> size_to_bytes(uint64_t size) {
    std::vector<uint8_t> b(8);
    for (int i = 0; i < 8; ++i) {
        b[i] = (size >> (i * 8)) & 0xFF;
    }
    return b;
}

// Helper to overwrite bytes at a specific index
void patch_size(std::vector<uint8_t> &bytes, size_t index, uint64_t size) {
    std::vector<uint8_t> s = size_to_bytes(size);
    for (int i = 0; i < 8; ++i) {
        bytes[index + i] = s[i];
    }
}

int main() {
    std::vector<uint8_t> bytes;

    gen_ELF(bytes); // Offset 0-63
    
    size_t header_start = bytes.size(); 
    
    gen_header(bytes, size_to_bytes(0)); 

    // section .text

    // print
    // ...

    // exit with code 0
    bytes.insert(bytes.end(), {inst::MOV_EAX, 60, 0, 0, 0});
    bytes.insert(bytes.end(), {inst::MOV_EDI, 0, 0, 0, 0});
    bytes.insert(bytes.end(), {inst::SYSCALL_1, inst::SYSCALL_2});

    end_of_text = bytes.size();

    // section .data
    bytes.insert(bytes.end(), {'H', 'i', '\n'});

    uint64_t final_size = bytes.size();
    
    patch_size(bytes, header_start + 32, final_size);

    patch_size(bytes, header_start + 40, final_size);

    std::ofstream file("code.bin", std::ios::binary);
    write_bytes(bytes, file);
    file.close();

    return 0;
}