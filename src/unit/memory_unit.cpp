#include "unit/memory_unit.h"
#include "storage/bus.h"
#include "simulator.h"

namespace conless {

void MemoryUnit::Flush(State *current_state) {
  if (!current_state->stall_) {
    current_state->input_ins_ = FetchWordUnsafe(current_state->pc_);
  }
  if (counter_ == 0) {
    if (mem_bus_->entries_.busy(0) && (mem_bus_->entries_[0].type_ == BusType::LoadRequest || mem_bus_->entries_[0].type_ == BusType::StoreRequest)) {
      counter_ = 2;
    }
  }
#ifdef SHOW_ALL
  printf("\tMemory counter: %d\n\n", counter_);
#endif
}

auto MemoryUnit::FetchWordUnsafe(AddrType pos) -> WordType {
  return ram_[pos] + (ram_[pos + 1] << 8) + (ram_[pos + 2] << 16) + (ram_[pos + 3] << 24);
}

void MemoryUnit::Execute(State *current_state, State *next_state) {
  if (counter_ == 0 || --counter_ != 0) {
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
      for (auto entry : mem_bus_->entries_) {
        ram_[entry.second.pos_] = entry.second.data_;
      }
      mem_bus_->entries_[0] = {BusType::StoreFinish, 0, 0};
      mem_bus_->entries_.busy(1) = false;
      mem_bus_->entries_.busy(2) = false;
      mem_bus_->entries_.busy(3) = false;
    } else {
      throw std::exception();
    }
  }
}

}