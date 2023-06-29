#include "unit/load_store_buffer.h"
#include <exception>
#include "common/types.h"
#include "simulator.h"
#include "storage/bus.h"
#include "utils/utils.h"

namespace conless {

void LoadStoreBuffer::Flush(State *current_state) {
  if (current_state->clean_) {
    load_buffer_.clean();
    if (current_state->st_req_.first) {
      auto store_entry = store_buffer_.pop();
      store_buffer_.clean();
      store_buffer_.push(store_entry);
    } else {
      store_buffer_.clean();
    }
    current_state->load_full_ = load_buffer_.full();
    current_state->store_full_ = store_buffer_.full();
    return;
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
      load_buffer_.push(new_entry);
    } else {
      throw std::exception();
    }
#ifdef SHOW_ALL
    printf("\tLSB receives @%d: %s %d bytes %s 0x%x\n", new_entry.rob_pos_,
           new_entry.type_ == OpcodeType::LOAD ? "LD" : "ST", new_entry.length_,
           new_entry.type_ == OpcodeType::LOAD ? "from" : "to", new_entry.start_addr_);
#endif
  }
  current_state->load_full_ = load_buffer_.full();
  current_state->store_full_ = store_buffer_.full();
  MonitorMemb();
#ifdef SHOW_ALL
  printf("\tCurrent load store buffer is:\n");
  for (auto entry : load_buffer_) {
    printf("\t\t@%-7d %s %d bytes from 0x%x\n", entry.rob_pos_, entry.type_ == OpcodeType::LOAD ? "LD" : "ST",
           entry.length_, entry.start_addr_);
  }
  for (auto entry : store_buffer_) {
    printf("\t\t@%-7d %s %d bytes to 0x%x\n", entry.rob_pos_, entry.type_ == OpcodeType::LOAD ? "LD" : "ST",
           entry.length_, entry.start_addr_);
  }
  printf("\n");
#endif
}

void LoadStoreBuffer::MonitorMemb() {
  if (!mem_bus_->entries_.busy(0)) {
    return;
  }
  auto &memb_entry = mem_bus_->entries_[0];
  if (memb_entry.type_ == BusType::LoadFinish) {
    auto load_entry = load_buffer_.pop();
    int data = mem_bus_->entries_[0].data_ + (mem_bus_->entries_[1].data_ << 8) + (mem_bus_->entries_[2].data_ << 16) +
               (mem_bus_->entries_[3].data_ << 24);
    if (load_entry.signed_tag_) {
      data = SignExtend(data, (load_entry.length_ << 3) - 1);
    }
    BusEntry bus_entry{BusType::WriteBack, load_entry.rob_pos_, data};
    temp_bus_entries_.push(bus_entry);
    mem_bus_->entries_.clean();
  } else if (memb_entry.type_ == BusType::StoreFinish) {
    mem_bus_->entries_.clean();
  }
}

void LoadStoreBuffer::Execute(State *current_state, State *next_state) {
  while (!temp_bus_entries_.empty()) {
    int space = cd_bus_->entries_.space();
    if (space == -1) {
      throw std::exception();
    }
    auto temp_entry = temp_bus_entries_.pop();
    cd_bus_->entries_[space] = temp_entry;
  }
  if (mem_bus_->entries_.busy(0)) {
    return;
  }
  if (current_state->st_req_.first) {
    auto store_entry = store_buffer_.pop();
    if (store_entry.rob_pos_ != current_state->st_req_.second.first) {
      throw std::exception();
    }
    int data = current_state->st_req_.second.second;
    if (store_entry.length_ == 1) {
      mem_bus_->entries_[0] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_), data & 0b11111111};
      mem_bus_->entries_.busy(1) = false;
      mem_bus_->entries_.busy(2) = false;
      mem_bus_->entries_.busy(3) = false;
    } else if (store_entry.length_ == 2) {
      mem_bus_->entries_[0] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_),
                               data & 0b11111111};
      mem_bus_->entries_[1] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_ + 1), data >> 8 & 0b11111111};
      mem_bus_->entries_.busy(2) = false;
      mem_bus_->entries_.busy(3) = false;
    } else if (store_entry.length_ == 4) {
      mem_bus_->entries_[0] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_),
                               data & 0b11111111};
      mem_bus_->entries_[1] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_ + 1),
                               data >> 8 & 0b11111111};
      mem_bus_->entries_[2] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_ + 2),
                               data >> 16 & 0b11111111};
      mem_bus_->entries_[3] = {BusType::StoreRequest, static_cast<int>(store_entry.start_addr_ + 3), data >> 24 & 0b11111111};
    }
    next_state->st_req_.first = false;
    return;
  }
  if (load_buffer_.empty()) {
    return;
  }
  auto &load_entry = load_buffer_.front();
  mem_bus_->entries_[0] = {BusType::LoadRequest, static_cast<int>(load_entry.start_addr_), load_entry.length_};
  mem_bus_->entries_.busy(1) = false;
  mem_bus_->entries_.busy(2) = false;
  mem_bus_->entries_.busy(3) = false;
}
}  // namespace conless