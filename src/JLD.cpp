

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include "machine-code.hpp"

std::vector<uint8_t> bytes;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "JLD: No input File" << std::endl;
  }

  std::fstream file(argv[1], std::ios::in | std::ios::binary);
  if (!file)
    std::cerr << "JLD: failed to open file" << std::endl;

  unsigned char byte;

  while (file.read(reinterpret_cast<char *>(&byte), 1))
    bytes.push_back(byte);

  bytes.insert(bytes.begin(), {0x7F, 'E', 'L', 'F'});

  for (auto byte : bytes)
    std::printf("%02X ", byte);

  std::printf("\n");

  file.close();
  return 0;
}