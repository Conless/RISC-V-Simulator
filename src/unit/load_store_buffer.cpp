#include "unit/load_store_buffer.h"
#include <exception>
#include "common/types.h"
#include "simulator.h"
#include "storage/bus.h"
#include "utils/utils.h"

namespace conless {

void LoadStoreBuffer::Flush(State *current_state) {
  if (current_state->clean_) {
    store_buffer_.clean();
    load_buffer_.clean();
  }
  if (current_state->lsb_entry_.first) {
    auto &new_entry = current_state->lsb_entry_.second;
    if (new_entry.type_ == OpcodeType::STORE) {
      if (store_buffer_.full()) {
        throw std::exception();
      }
      store_buffer_.push(new_entry);
    } else if (new_entry.type_ == OpcodeType::LOAD) {
      if (load_buffer_.full()) {
        throw std::exception();
      }
      for (auto entry : store_buffer_) {
        int entry_end_addr = entry.start_addr_ + entry.length_ - 1;
        int new_entry_end_addr = new_entry.start_addr_ + new_entry.length_ - 1;
        if ((entry_end_addr < new_entry.start_addr_ || entry.start_addr_ > new_entry_end_addr) ||
            entry.rob_pos_ > new_entry.rob_pos_) {
          continue;
        }
        if (entry.start_addr_ == new_entry.start_addr_ && entry_end_addr == new_entry_end_addr) {
          temp_bus_entries_.push({BusType::SideChannel, new_entry.rob_pos_, entry.rob_pos_});
          return;
        }
        throw std::exception();
      }
      store_buffer_.push(new_entry);
    } else {
      throw std::exception();
    }
  }
}

void LoadStoreBuffer::MonitorMemb() {
  if (!mem_bus_->entries_.busy(0)) {
    return;
  }
  auto &memb_entry = mem_bus_->entries_[0];
  if (memb_entry.type_ == BusType::LoadFinish) {
    auto store_entry = store_buffer_.pop();
    int data = mem_bus_->entries_[0].data_ + (mem_bus_->entries_[1].data_ << 8) + (mem_bus_->entries_[2].data_ << 16) +
               (mem_bus_->entries_[3].data_ << 24);
    if (store_entry.signed_tag_) {
      data = SignExtend(data, store_entry.length_ << 3);
    }
    BusEntry bus_entry{BusType::WriteBack, store_entry.rob_pos_, memb_entry.data_};
    temp_bus_entries_.push(bus_entry);
  }
  mem_bus_->entries_.busy(0) = false;
  mem_bus_->entries_.busy(1) = false;
  mem_bus_->entries_.busy(2) = false;
  mem_bus_->entries_.busy(3) = false;
}

void LoadStoreBuffer::Execute(State *current_state, State *next_state) {
  while (!temp_bus_entries_.empty()) {
    auto temp_entry = temp_bus_entries_.pop();
    int space = cd_bus_->entries_.space();
    if (space == -1) {
      throw std::exception();
    }
    cd_bus_->entries_[space] = temp_entry;
  }
  if (mem_bus_->entries_.busy(0)) {
    return;
  }
  if (!current_state->st_req_.empty()) {
    auto &store_entry = store_buffer_.front();
    int data = current_state->st_req_.front();
    if (store_entry.length_ == 1) {
      mem_bus_->entries_[0] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_), data & 0b11111111};
      mem_bus_->entries_.busy(1) = false;
      mem_bus_->entries_.busy(2) = false;
      mem_bus_->entries_.busy(3) = false;
    } else if (store_entry.length_ == 2) {
      mem_bus_->entries_[0] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_),
                               data >> 8 & 0b11111111};
      mem_bus_->entries_[1] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_ + 1), data & 0b11111111};
      mem_bus_->entries_.busy(2) = false;
      mem_bus_->entries_.busy(3) = false;
    } else if (store_entry.length_ == 4) {
      mem_bus_->entries_[0] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_),
                               data >> 24 & 0b11111111};
      mem_bus_->entries_[1] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_ + 1),
                               data >> 16 & 0b11111111};
      mem_bus_->entries_[2] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_ + 2),
                               data >> 8 & 0b11111111};
      mem_bus_->entries_[3] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_ + 3), data & 0b11111111};
    }
    return;
  }
  auto &load_entry = load_buffer_.front();
  mem_bus_->entries_[0] = {BusType::LoadRequest, static_cast<int>(load_entry.start_addr_), load_entry.length_};
  mem_bus_->entries_.busy(1) = false;
  mem_bus_->entries_.busy(2) = false;
  mem_bus_->entries_.busy(3) = false;
}
}  // namespace conless