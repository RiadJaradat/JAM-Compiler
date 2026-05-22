#pragma once

#include "helper.hpp"
#include "machine-code.hpp"

#define CLOSE 1
#define LEAVE 0

class generall_jam_emmiter {
private:
  std::map<std::vector<uint8_t>, size_t> program_table;
  std::map<size_t, size_t> ptr_size;
  std::map<size_t, size_t> Addresses;
  std::map<std::string, size_t> labels;

public:
  mc::x86_64::ELF elf;
  mc::x86_64::Header program_header;
  mc::x86_64::machine_code code_bytes;
  mc::x86_64::machine_code data_bytes;

  generall_jam_emmiter() {

  };

  void genLabel(std::string label_name) {
    labels[label_name] = code_bytes.size();
  }

  void print(std::string value) {
    helper::__print(code_bytes, program_table, ptr_size, value);
  }

  void end(int return_type) {
    helper::__exit(code_bytes, return_type);
  }

  void genExcutable(std::ofstream &FILE, std::string out_name, int after_gen = LEAVE) {

    helper::patch(code_bytes, data_bytes, program_table, ptr_size, Addresses, program_header);
    mc::x86_64::machine_code bytes = helper::combin_excutable(elf, program_header, code_bytes, data_bytes);
    helper::write(FILE, bytes);

    switch (after_gen) {
        case LEAVE: break;
        case CLOSE: {
            FILE.close(); 
            std::string command = "chmod +x " + out_name;
            system(command.c_str());      
            break;
        }
        default: break;
    }

  }
};

typedef generall_jam_emmiter Emmiter;