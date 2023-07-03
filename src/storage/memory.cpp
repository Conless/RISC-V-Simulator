#include "storage/memory.h"
#include <iostream>
#include "common/config.h"
#include "common/types.h"
#include "simulator.h"
#include "storage/bus.h"
#include "utils/utils.h"

namespace conless {

inline auto HexToInt(char ch) -> int { return ch <= '9' ? ch - '0' : (ch - 'A') + 10; }

void Memory::Init() {
  char input_str[10];
  AddrType current_address = 0x0;
  while (std::cin >> input_str) {
    if (input_str[0] == '@') {
      current_address = 0x0;
      for (int i = 1; i <= 8; i++) {
        current_address = (current_address << 4) + HexToInt(input_str[i]);
      }
    } else {
      ByteType current_data = (HexToInt(input_str[0]) << 4) + HexToInt(input_str[1]);
      ram_[current_address++] = current_data;
    }
  }
}

auto Memory::operator[](const size_t pos) -> ByteType & {
  if (pos >= MAX_RAM_SIZE) {
    throw std::exception();
  }
  return ram_[pos];
}


}  // namespace conless