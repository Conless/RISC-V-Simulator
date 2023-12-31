#include "unit/arithmetic_logic_unit.h"
#include <exception>

#include "common/types.h"
#include "simulator.h"
#include "storage/bus.h"
#include "utils/utils.h"

namespace conless {

void ArithmeticLogicUnit::Flush(State *current_state) {
  if (current_state->clean_) {
    entries_.clean();
    return;
  }
  if (current_state->alu_entry_.first) {
    if (entries_.full()) {
      throw std::exception();
    }
    entries_.push(current_state->alu_entry_.second);
#ifdef SHOW_ALL
    printf("\tALU receives @%d: %s %x %x\n", current_state->alu_entry_.second.rob_pos_,
           OpcodeToString(current_state->alu_entry_.second.opcode_).c_str(), current_state->alu_entry_.second.lhs_,
           current_state->alu_entry_.second.rhs_);
#endif
  }
}

void ArithmeticLogicUnit::Execute(State *current_state, State *next_state) {
  if (entries_.empty()) {
    return;
  }
  AluEntry alu_entry = entries_.pop();
  BusEntry bus_entry{WriteBack, alu_entry.rob_pos_};
  if (alu_entry.opcode_ == Opcode::BEQ) {
    bus_entry.data_ = static_cast<int>(alu_entry.lhs_ == alu_entry.rhs_);
  } else if (alu_entry.opcode_ == Opcode::BNE) {
    bus_entry.data_ = static_cast<int>(alu_entry.lhs_ != alu_entry.rhs_);
  } else if (alu_entry.opcode_ == Opcode::BLT || alu_entry.opcode_ == Opcode::SLTI ||
             alu_entry.opcode_ == Opcode::SLT) {
    bus_entry.data_ = static_cast<int>(alu_entry.lhs_ < alu_entry.rhs_);
  } else if (alu_entry.opcode_ == Opcode::BLTU || alu_entry.opcode_ == Opcode::SLTIU ||
             alu_entry.opcode_ == Opcode::SLTU) {
    bus_entry.data_ = static_cast<int>(static_cast<unsigned>(alu_entry.lhs_) < static_cast<unsigned>(alu_entry.rhs_));
  } else if (alu_entry.opcode_ == Opcode::BGE) {
    bus_entry.data_ = static_cast<int>(alu_entry.lhs_ >= alu_entry.rhs_);
  } else if (alu_entry.opcode_ == Opcode::BGEU) {
    bus_entry.data_ = static_cast<int>(static_cast<unsigned>(alu_entry.lhs_) >= static_cast<unsigned>(alu_entry.rhs_));
  } else if (alu_entry.opcode_ == Opcode::ADDI || alu_entry.opcode_ == Opcode::ADD) {
    bus_entry.data_ = alu_entry.lhs_ + alu_entry.rhs_;
  } else if (alu_entry.opcode_ == Opcode::SUB) {
    bus_entry.data_ = alu_entry.lhs_ - alu_entry.rhs_;
  } else if (alu_entry.opcode_ == Opcode::ANDI || alu_entry.opcode_ == Opcode::AND) {
    bus_entry.data_ = alu_entry.lhs_ & alu_entry.rhs_;
  } else if (alu_entry.opcode_ == Opcode::ORI || alu_entry.opcode_ == Opcode::OR) {
    bus_entry.data_ = alu_entry.lhs_ | alu_entry.rhs_;
  } else if (alu_entry.opcode_ == Opcode::XORI || alu_entry.opcode_ == Opcode::XOR) {
    bus_entry.data_ = alu_entry.lhs_ ^ alu_entry.rhs_;
  } else if (alu_entry.opcode_ == Opcode::SLLI || alu_entry.opcode_ == Opcode::SLL) {
    bus_entry.data_ = alu_entry.lhs_ << alu_entry.rhs_;
  } else if (alu_entry.opcode_ == Opcode::SRAI || alu_entry.opcode_ == Opcode::SRA) {
    bus_entry.data_ = alu_entry.lhs_ >> alu_entry.rhs_;
  } else if (alu_entry.opcode_ == Opcode::SRLI || alu_entry.opcode_ == Opcode::SRL) {
    bus_entry.data_ = static_cast<int>(static_cast<unsigned>(alu_entry.lhs_) >> static_cast<unsigned>(alu_entry.rhs_));
  } else if (alu_entry.opcode_ == Opcode::JALR) {
    bus_entry.data_ = (alu_entry.lhs_ + alu_entry.rhs_) & (~1);
  } else {
    throw std::exception();
  }
  int space = cd_bus_->entries_.space();
  if (space == -1) {
    throw std::exception();
  }
  cd_bus_->entries_[space] = bus_entry;
}

}  // namespace conless