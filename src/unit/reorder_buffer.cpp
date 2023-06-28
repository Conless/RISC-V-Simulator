#include "unit/reorder_buffer.h"
#include <exception>
#include "common/types.h"
#include "simulator.h"
#include "storage/bus.h"
#include "utils/utils.h"

namespace conless {

void ReorderBuffer::Flush(State *current_state) {
  if (current_state->clean_) {
    entries_.clean();
    return;
  }
  if (current_state->rob_entry_.first) {
    if (entries_.full()) {
      throw std::exception();
    }
#ifdef DEBUG
    printf("Reorder buffer receives: ");
    DisplayIns(current_state->rob_entry_.second.ins_);
#endif
    entries_.push(current_state->rob_entry_.second);
  }
  current_state->rob_full_ = entries_.full();
  current_state->rob_tail_ = entries_.tail() + 1;
  MonitorCdb();
}

void ReorderBuffer::Execute(State *current_state, State *next_state) {
  Commit(current_state, next_state);
}

void ReorderBuffer::MonitorCdb() {
  for (auto bus_entry : cd_bus_->entries_) {
    if (bus_entry.second.type_ == BusType::Executing) {
      entries_[bus_entry.second.pos_].state_ = RobState::Exec;
    } else if (bus_entry.second.type_ == BusType::WriteBack) {
      entries_[bus_entry.second.pos_].state_ = RobState::Write;
      entries_[bus_entry.second.pos_].value_ = bus_entry.second.data_;
    } else if (bus_entry.second.type_ == BusType::SideChannel) {
      entries_[bus_entry.second.pos_].state_ = RobState::Side;
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
    } else {
      // TODO(Conless): prediction
    }
  } else if (entry.ins_.opcode_type_ == OpcodeType::ARITHI || entry.ins_.opcode_type_ == OpcodeType::ARITHR ||
             entry.ins_.opcode_type_ == OpcodeType::LOAD) {
    next_state->reg_file_.regs_[entry.ins_.rd_].data_ = entry.value_;
  } else if (entry.ins_.opcode_type_ == OpcodeType::STORE) {
    if (current_state->st_req_.full()) {
      return;
    }
    entry.value_ = current_state->reg_file_.regs_[entry.ins_.rs2_].data_;
    for (auto next_entry : entries_) {
      if (next_entry.state_ == RobState::Side && next_entry.value_ == entries_.head()) {
        next_entry.state_ = RobState::Write;
        next_entry.value_ = entry.value_;
      }
    }
    current_state->st_req_.push(entry.value_);
  } else {
    throw std::exception();
  }
  entries_.pop();
}

}  // namespace conless