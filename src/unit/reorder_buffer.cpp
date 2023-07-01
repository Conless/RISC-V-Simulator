#include "unit/reorder_buffer.h"
#include <exception>
#include "common/config.h"
#include "common/types.h"
#include "simulator.h"
#include "storage/bus.h"
#include "utils/utils.h"

namespace conless {

void ReorderBuffer::Flush(State *current_state) {
  if (current_state->clean_) {
    entries_.clean();
    current_state->rob_full_ = entries_.full();
    current_state->rob_tail_ = entries_.tail() + 1;
    return;
  }
  if (current_state->rob_entry_.first) {
    if (entries_.full()) {
      throw std::exception();
    }
    entries_.push(current_state->rob_entry_.second);
#ifdef SHOW_ALL
    printf("\tReorder buffer receives: %s\n", InsToString(current_state->rob_entry_.second.ins_).c_str());
#endif
  }
  current_state->rob_full_ = entries_.full();
  current_state->rob_tail_ = entries_.tail() + 1;
  MonitorCdb(current_state);
#ifdef SHOW_ALL
  printf("\tCurrent reorder buffer is:\n");
  int i = entries_.head();
  for (auto entry : entries_) {
    printf("\t\t@%-7d (%04x)  %-20s   %-8s   %-7d %-7x\n", i++, entry.ins_addr_, InsToString(entry.ins_).c_str(),
           RobStateToString(entry.state_).c_str(), entry.dest_, entry.value_);
  }
  printf("\n");
#endif
}

void ReorderBuffer::Execute(State *current_state, State *next_state) { Commit(current_state, next_state); }

void ReorderBuffer::MonitorCdb(State *current_state) {
  for (auto bus_entry : cd_bus_->entries_) {
    if (bus_entry.second.type_ == BusType::Executing) {
      entries_[bus_entry.second.pos_].state_ = RobState::Exec;
    } else if (bus_entry.second.type_ == BusType::WriteBack) {
      entries_[bus_entry.second.pos_].state_ = RobState::Write;
      entries_[bus_entry.second.pos_].value_ = bus_entry.second.data_;
    } else if (bus_entry.second.type_ == BusType::SideChannel) {
      if (bus_entry.second.data_ == current_state->st_req_.second.first) {
        entries_[bus_entry.second.pos_].state_ = RobState::Write;
        entries_[bus_entry.second.pos_].value_ = current_state->st_req_.second.second;
      } else {
        entries_[bus_entry.second.pos_].state_ = RobState::Side;
        entries_[bus_entry.second.pos_].value_ = bus_entry.second.data_;
      }
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
  if (entry.ins_.opcode_ == Opcode::ADDI && entry.ins_.rd_ == 10 && entry.ins_.rs1_ == 0 && entry.ins_.imm_ == 255) {
    next_state->terminate_ = true;
    return;
  }
  if (entry.ins_.opcode_ == Opcode::JALR) {
    next_state->reg_file_.regs_[entry.ins_.rd_] = {static_cast<int>(entry.ins_addr_), -1};
    next_state->pc_ = entry.value_;
    next_state->stall_ = false;
  } else if (entry.ins_.opcode_type_ == OpcodeType::BRANCH) {
    predictor_->PredictFeedBack(entry.ins_addr_, entry.ins_.rd_ != 0, entry.value_ != 0);
    if (entry.ins_.rd_ != entry.value_) {
#ifdef SHOW_PC
      printf("Predict wrong at %x.\n", entry.ins_.ins_addr_);
#endif
      next_state->clean_ = true;
      next_state->pc_ = entry.value_ != 0 ? entry.ins_addr_ + entry.ins_.imm_ : entry.ins_addr_ + 4;
    }
  } else if (entry.ins_.opcode_type_ == OpcodeType::ARITHI || entry.ins_.opcode_type_ == OpcodeType::ARITHR ||
             entry.ins_.opcode_type_ == OpcodeType::LOAD) {
    int space = cd_bus_->entries_.space();
    if (space == -1) {
      return;
    }
    next_state->reg_file_.regs_[entry.ins_.rd_].data_ = entry.value_;
    if (next_state->reg_file_.regs_[entry.ins_.rd_].dependency_ == entry.rob_pos_) {
      next_state->reg_file_.regs_[entry.ins_.rd_].dependency_ = -1;
    }
    cd_bus_->entries_[space] = {BusType::CommitReg, entry.rob_pos_, entry.value_};
  } else if (entry.ins_.opcode_type_ == OpcodeType::STORE) {
    if (current_state->st_req_.first) {
      return;
    }
    entry.value_ = current_state->reg_file_.regs_[entry.ins_.rs2_].data_;
    for (auto next_entry : entries_) {
      if (next_entry.state_ == RobState::Side && next_entry.value_ == entry.rob_pos_) {
        int space = cd_bus_->entries_.space();
        if (space == -1) {
          return;
        }
        cd_bus_->entries_[space] = {BusType::WriteBack, next_entry.rob_pos_, entry.value_};
      }
    }
    next_state->st_req_ = {true, {entry.rob_pos_, entry.value_}};
  } else {
    throw std::exception();
  }
#ifdef SHOW_REG
  printf("Finish executing pc %x with regs:\n", entry.ins_addr_);
  for (int i = 1; i < REG_FILE_SIZE; i++) {
    auto &reg = next_state->reg_file_.regs_[i];
    if (reg.data_ != 0) {
      printf("[%02d]:%-8x", i, reg.data_);
    }
  }
  printf("\n");
#endif
  entries_.pop();
}

}  // namespace conless