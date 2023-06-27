#include "unit/arithmetic_logic_unit.h"
#include <exception>

#include "common/types.h"
#include "simulator.h"
#include "storage/bus.h"

namespace conless {

void ArithmeticLogicUnit::Flush(State *current_state) {
  if (current_state->alu_entry_.first) {
    if (entries_.full()) {
      throw std::exception();
    }
    entries_.push(current_state->alu_entry_.second);
  }
}

void ArithmeticLogicUnit::Execute() {
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
    bus_entry.data_ = static_cast<int>((alu_entry.lhs_ < 0) ||
                                       (static_cast<unsigned>(alu_entry.lhs_) < static_cast<unsigned>(alu_entry.rhs_)));
  } else if (alu_entry.opcode_ == Opcode::BGE) {
    bus_entry.data_ = static_cast<int>(alu_entry.lhs_ >= alu_entry.rhs_);
  } else if (alu_entry.opcode_ == Opcode::BGEU) {
    bus_entry.data_ = static_cast<int>(
        alu_entry.lhs_ >= 0 && (static_cast<unsigned>(alu_entry.lhs_) >= static_cast<unsigned>(alu_entry.rhs_)));
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
  } else {
    throw std::exception();
  }
  int space = cdb_->entries_.space();
  if (space == -1) {
    throw std::exception();
  }
  cdb_->entries_[space] = bus_entry;
}

}  // namespace conless