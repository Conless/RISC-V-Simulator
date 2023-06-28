#include "storage/memory.h"
#include <iostream>
#include "common/types.h"
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

void Memory::Flush(State *current_state) {
  if (counter_ == 0) {
    if (mem_bus_->entries_[0].type_ == BusType::LoadRequest || mem_bus_->entries_[0].type_ == BusType::StoreRequest) {
      counter_ = 3;
    }
  }
#ifdef DEBUG
  printf("\tMemory counter: %d\n", counter_);
#endif
}

auto Memory::FetchWordUnsafe(AddrType pos) -> WordType {
  return ram_[pos] + (ram_[pos + 1] << 8) + (ram_[pos + 2] << 16) + (ram_[pos + 3] << 24);
}

void Memory::Execute(State *current_state, State *next_state) {
  if (--counter_ != 0) {
    return;
  }
  if (mem_bus_->entries_.busy(0)) {
    if (mem_bus_->entries_[0].type_ == BusType::LoadRequest) {
      int length = mem_bus_->entries_[0].data_;
      int pos = mem_bus_->entries_[0].pos_;
      if (length == 1) {
        mem_bus_->entries_[0] = {BusType::LoadFinish, 0, ram_[pos]};
        mem_bus_->entries_[1] = {BusType::LoadFinish, 0, 0};
        mem_bus_->entries_[2] = {BusType::LoadFinish, 0, 0};
        mem_bus_->entries_[3] = {BusType::LoadFinish, 0, 0};
      } else if (length == 2) {
        mem_bus_->entries_[0] = {BusType::LoadFinish, 0, ram_[pos]};
        mem_bus_->entries_[1] = {BusType::LoadFinish, 0, ram_[pos + 1]};
        mem_bus_->entries_[2] = {BusType::LoadFinish, 0, 0};
        mem_bus_->entries_[3] = {BusType::LoadFinish, 0, 0};
      } else if (length == 4) {
        mem_bus_->entries_[0] = {BusType::LoadFinish, 0, ram_[pos]};
        mem_bus_->entries_[1] = {BusType::LoadFinish, 0, ram_[pos + 1]};
        mem_bus_->entries_[2] = {BusType::LoadFinish, 0, ram_[pos + 2]};
        mem_bus_->entries_[3] = {BusType::LoadFinish, 0, ram_[pos + 3]};
      }
    } else if (mem_bus_->entries_[0].type_ == BusType::StoreRequest) {
      // for (auto entry : mem_bus_->entries_) {
      //   ram_[entry.second.pos_] = entry.second.data_;
      // }
      mem_bus_->entries_[0] = {BusType::StoreFinish, 0, 0};
      mem_bus_->entries_.busy(1) = false;
      mem_bus_->entries_.busy(2) = false;
      mem_bus_->entries_.busy(3) = false;
    } else {
      throw std::exception();
    }
  }
}

}  // namespace conless