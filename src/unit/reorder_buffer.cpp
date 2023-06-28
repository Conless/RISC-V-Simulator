#include "unit/reorder_buffer.h"
#include <exception>
#include "common/types.h"
#include "simulator.h"
#include "storage/bus.h"

namespace conless {

void ReorderBuffer::Flush(State *current_state) {
  if (current_state->rob_entry_.first) {
    if (entries_.full()) {
      throw std::exception();
    }
    entries_.push(current_state->rob_entry_.second);
  }
  current_state->rob_full_ = entries_.full();
  current_state->rob_tail_ = entries_.tail() + 1;
}

void ReorderBuffer::MonitorCdb() {
  for (auto bus_entry : cd_bus_->entries_) {
    if (bus_entry.second.type_ == BusType::Executing) {
      entries_[bus_entry.second.pos_].state_ = RobState::Exec;
    } else if (bus_entry.second.type_ == BusType::WriteBack) {
      entries_[bus_entry.second.pos_].state_ = RobState::Exec;
      entries_[bus_entry.second.pos_].value_ = bus_entry.second.data_;
    }
  }
}

void ReorderBuffer::Commit(State *current_state, State *next_state) {
  if (entries_.empty()) {
    return;
  }
  auto &entry = entries_.front();
  if (entry.state_ != RobState::Write) {
    return;
  }
  if (entry.ins_.opcode_ == Opcode::JALR) {
    next_state->reg_file_.regs_[entry.ins_.rd_] = {static_cast<int>(entry.ins_addr_), -1};
    next_state->pc_ = entry.value_;  // TODO(conless): fix it
  } else if (entry.ins_.opcode_type_ == OpcodeType::BRANCH) {
    if (entry.ins_.rd_ != entry.value_) {
      next_state->clean_ = true;
    }
  } else if (entry.ins_.opcode_type_ == OpcodeType::ARITHI || entry.ins_.opcode_type_ == OpcodeType::ARITHR ||
             entry.ins_.opcode_type_ == OpcodeType::LOAD) {
    next_state->reg_file_.regs_[entry.ins_.rd_].data_ = entry.value_;
  } else if (entry.ins_.opcode_type_ == OpcodeType::STORE) {
    if (current_state->st_req_.full()) {
      return;
    }
    current_state->st_req_.push(current_state->reg_file_.regs_[entry.ins_.rs2_].data_);
  } else {
    throw std::exception();
  }
  entries_.pop();
}

}  // namespace conless