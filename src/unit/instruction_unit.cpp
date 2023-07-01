#include "unit/instruction_unit.h"
#include <cstdint>
#include <exception>
#include "common/types.h"
#include "simulator.h"
#include "unit/reorder_buffer.h"
#include "unit/reservation_station.h"
#include "utils/utils.h"

namespace conless {

void DecodeLoad(WordType input_ins, InsType &ins) {
  int load_type = input_ins >> 12 & 0b11;
  bool signed_type = (input_ins >> 14 & 1) == 0U;
  ins.rs1_ = input_ins >> 15 & 0b11111;
  ins.rd_ = input_ins >> 7 & 0b11111;
  ins.imm_ = SignExtend(input_ins >> 20, 11);
  if (load_type == 0b00) {
    ins.opcode_ = signed_type ? Opcode::LB : Opcode::LBU;
  } else if (load_type == 0b01) {
    ins.opcode_ = signed_type ? Opcode::LH : Opcode::LHU;
  } else if (load_type == 0b10) {
    ins.opcode_ = Opcode::LW;
  } else {
    throw std::exception();
  }
}

void DecodeStore(WordType input_ins, InsType &ins) {
  int store_type = input_ins >> 12 & 0b11;
  ins.rs1_ = input_ins >> 15 & 0b11111;
  ins.rs2_ = input_ins >> 20 & 0b11111;
  ins.imm_ = SignExtend(((input_ins >> 25) << 5) + ((input_ins >> 7) & 0b11111), 11);
  if (store_type == 0b00) {
    ins.opcode_ = Opcode::SB;
  } else if (store_type == 0b01) {
    ins.opcode_ = Opcode::SH;
  } else if (store_type == 0b10) {
    ins.opcode_ = Opcode::SW;
  } else {
    throw std::exception();
  }
}

void DecodeArithI(WordType input_ins, InsType &ins) {
  int arith_type = input_ins >> 12 & 0b111;
  ins.rs1_ = input_ins >> 15 & 0b11111;
  ins.rd_ = input_ins >> 7 & 0b11111;
  if (arith_type == 0b000) {
    ins.opcode_ = Opcode::ADDI;
  } else if (arith_type == 0b010) {
    ins.opcode_ = Opcode::SLTI;
  } else if (arith_type == 0b011) {
    ins.opcode_ = Opcode::SLTIU;
  } else if (arith_type == 0b100) {
    ins.opcode_ = Opcode::XORI;
  } else if (arith_type == 0b110) {
    ins.opcode_ = Opcode::ORI;
  } else if (arith_type == 0b111) {
    ins.opcode_ = Opcode::ANDI;
  } else {
    bool tag_type = (input_ins >> 30 & 0b1) == 0U;
    ins.imm_ = input_ins >> 20 & 0b11111;
    if (arith_type == 0b001) {
      ins.opcode_ = Opcode::SLLI;
    } else if (arith_type == 0b101) {
      ins.opcode_ = tag_type ? Opcode::SRLI : Opcode::SRAI;
    } else {
      throw std::exception();
    }
    return;
  }
  ins.imm_ = SignExtend(input_ins >> 20, 11);
}

void DecodeArithR(WordType input_ins, InsType &ins) {
  int arith_type = input_ins >> 12 & 0b111;
  bool tag_type = (input_ins >> 30 & 0b1) == 0U;
  ins.rs1_ = input_ins >> 15 & 0b11111;
  ins.rs2_ = input_ins >> 20 & 0b11111;
  ins.rd_ = input_ins >> 7 & 0b11111;
  if (arith_type == 0b000) {
    ins.opcode_ = tag_type ? Opcode::ADD : Opcode::SUB;
  } else if (arith_type == 0b001) {
    ins.opcode_ = Opcode::SLL;
  } else if (arith_type == 0b010) {
    ins.opcode_ = Opcode::SLT;
  } else if (arith_type == 0b011) {
    ins.opcode_ = Opcode::SLTU;
  } else if (arith_type == 0b100) {
    ins.opcode_ = Opcode::XOR;
  } else if (arith_type == 0b101) {
    ins.opcode_ = tag_type ? Opcode::SRL : Opcode::SRA;
  } else if (arith_type == 0b110) {
    ins.opcode_ = Opcode::OR;
  } else if (arith_type == 0b111) {
    ins.opcode_ = Opcode::AND;
  } else {
    throw std::exception();
  }
}

void DecodeBranch(WordType input_ins, InsType &ins) {
  int branch_type = input_ins >> 12 & 0b111;
  ins.rs1_ = input_ins >> 15 & 0b11111;
  ins.rs2_ = input_ins >> 20 & 0b11111;
  ins.imm_ = SignExtend(((input_ins >> 31 & 0b1) << 12) + ((input_ins >> 7 & 0b1) << 11) +
                            ((input_ins >> 25 & 0b111111) << 5) + ((input_ins >> 8 & 0b1111) << 1),
                        12);

  if (branch_type == 0b000) {
    ins.opcode_ = Opcode::BEQ;
  } else if (branch_type == 0b001) {
    ins.opcode_ = Opcode::BNE;
  } else if (branch_type == 0b100) {
    ins.opcode_ = Opcode::BLT;
  } else if (branch_type == 0b101) {
    ins.opcode_ = Opcode::BGE;
  } else if (branch_type == 0b110) {
    ins.opcode_ = Opcode::BLTU;
  } else if (branch_type == 0b111) {
    ins.opcode_ = Opcode::BGEU;
  } else {
    throw std::exception();
  }
}

void DecodeOther(WordType input_ins, InsType &ins) {
  int opcode_num = input_ins & 0b1111111;
  ins.rd_ = input_ins >> 7 & 0b11111;
  if (opcode_num == 0b1101111) {
    ins.opcode_ = Opcode::JAL;
    ins.imm_ = SignExtend(((input_ins >> 31 & 0b1) << 20) + ((input_ins >> 12 & 0b11111111) << 12) +
                              ((input_ins >> 20 & 0b1) << 11) + ((input_ins >> 21 & 0b1111111111) << 1),
                          20);
  } else if (opcode_num == 0b1100111) {
    ins.opcode_ = Opcode::JALR;
    ins.rs1_ = input_ins >> 15 & 0b11111;
    ins.imm_ = SignExtend(input_ins >> 20, 11);
  } else if (opcode_num == 0b0010111) {
    ins.opcode_ = Opcode::AUIPC;
    ins.imm_ = SignExtend((input_ins >> 12) << 12, 31);
  } else if (opcode_num == 0b0110111) {
    ins.opcode_ = Opcode::LUI;
    ins.imm_ = SignExtend((input_ins >> 12) << 12, 31);
  } else {
    throw std::exception();
  }
}

void Decode(WordType input_ins, InsType &ins) {
  int opcode_num = input_ins & 0b1111111;
  if (opcode_num == 0b0000011) {
    ins.opcode_type_ = OpcodeType::LOAD;
    DecodeLoad(input_ins, ins);
    return;
  }
  if (opcode_num == 0b0100011) {
    ins.opcode_type_ = OpcodeType::STORE;
    DecodeStore(input_ins, ins);
    return;
  }
  if (opcode_num == 0b0010011) {
    ins.opcode_type_ = OpcodeType::ARITHI;
    DecodeArithI(input_ins, ins);
    return;
  }
  if (opcode_num == 0b0110011) {
    ins.opcode_type_ = OpcodeType::ARITHR;
    DecodeArithR(input_ins, ins);
    return;
  }
  if (opcode_num == 0b1100011) {
    ins.opcode_type_ = OpcodeType::BRANCH;
    DecodeBranch(input_ins, ins);
    return;
  }
  ins.opcode_type_ = OpcodeType::OTHER;
  DecodeOther(input_ins, ins);
}

void InstructionUnit::FetchDecode(State *current_state, State *next_state, WordType input_ins) {
  InsType ins;
  ins.ins_addr_ = current_state->pc_;
  if (current_state->ins_queue_full_ || current_state->stall_ || next_state->clean_) {
    return;
  }
  if (input_ins == 0x0ff00513) {
    next_state->stall_ = true;
  }
  Decode(input_ins, ins);
  if (ins.opcode_ == AUIPC) {
    next_state->pc_ = current_state->pc_ + ins.imm_;
    return;
  }
  if (ins.opcode_type_ == OpcodeType::BRANCH) {
    ins.rd_ = static_cast<int>(predictor_->GetPredictResult(current_state->pc_));
    if (ins.rd_ != 0) {
      next_state->pc_ = current_state->pc_ + ins.imm_;
    } else {
      next_state->pc_ = current_state->pc_ + 4;
    }
  } else if (ins.opcode_ == JAL) {
    next_state->pc_ = current_state->pc_ + ins.imm_;
    ins = {ARITHI, ADDI, 0, -1, ins.rd_, static_cast<int>(current_state->pc_ + 4), ins.ins_addr_};
  } else if (ins.opcode_ == JALR) {
    next_state->stall_ = true;
  } else {
    if (ins.opcode_ == LUI) {
      ins = {ARITHI, ADDI, 0, -1, ins.rd_, ins.imm_, ins.ins_addr_};
    }
    next_state->pc_ = current_state->pc_ + 4;
  }
  next_state->ins_reg_ = {true, ins};
}

void InstructionUnit::Issue(State *current_state, State *next_state) {
  if (ins_queue_.empty() || current_state->rob_full_) {
    return;
  }
  InsType ins = ins_queue_.front();
  RobEntry next_rob_entry{ins, RobState::Issue, current_state->rob_tail_, ins.ins_addr_, ins.rd_};
  RssEntry next_rss_entry{ins, current_state->rob_tail_, ins.imm_};
  if (ins.opcode_type_ == OpcodeType::LOAD || ins.opcode_type_ == OpcodeType::STORE) {
    if (current_state->ls_full_) {
      return;
    }
    if (current_state->reg_file_.regs_[ins.rs1_].dependency_ == -1) {
      next_rss_entry.v1_ = current_state->reg_file_.regs_[ins.rs1_].data_;
    } else {
      next_rss_entry.q1_ = current_state->reg_file_.regs_[ins.rs1_].dependency_;
    }
  } else {
    if (current_state->arith_full_) {
      return;
    }
    if (ins.opcode_type_ == OpcodeType::ARITHI || ins.opcode_ == Opcode::JALR) {
      if (current_state->reg_file_.regs_[ins.rs1_].dependency_ == -1) {
        next_rss_entry.v1_ = current_state->reg_file_.regs_[ins.rs1_].data_;
      } else {
        next_rss_entry.q1_ = current_state->reg_file_.regs_[ins.rs1_].dependency_;
      }
    } else if (ins.opcode_type_ == OpcodeType::ARITHR || ins.opcode_type_ == OpcodeType::BRANCH) {
      if (current_state->reg_file_.regs_[ins.rs1_].dependency_ == -1) {
        next_rss_entry.v1_ = current_state->reg_file_.regs_[ins.rs1_].data_;
      } else {
        next_rss_entry.q1_ = current_state->reg_file_.regs_[ins.rs1_].dependency_;
      }
      if (current_state->reg_file_.regs_[ins.rs2_].dependency_ == -1) {
        next_rss_entry.v2_ = current_state->reg_file_.regs_[ins.rs2_].data_;
      } else {
        next_rss_entry.q2_ = current_state->reg_file_.regs_[ins.rs2_].dependency_;
      }
    } else {
      throw std::exception();
    }
  }
  if (ins.opcode_type_ != OpcodeType::BRANCH && ins.rd_ != -1) {
    next_state->reg_file_.regs_[ins.rd_].dependency_ = current_state->rob_tail_;
  }
  ins_queue_.pop();
  next_state->rob_entry_ = {true, next_rob_entry};
  next_state->rss_entry_ = {true, next_rss_entry};
}

void InstructionUnit::Flush(State *current_state) {
  if (current_state->clean_) {
    ins_queue_.clean();
    current_state->ins_queue_full_ = ins_queue_.full();
  }
  if (current_state->ins_reg_.first) {
    if (ins_queue_.full()) {
      throw std::exception();
    }
    ins_queue_.push(current_state->ins_reg_.second);
#ifdef SHOW_ALL
    printf("\tInstruction queue receives: %s\n\n", InsToString(current_state->ins_reg_.second).c_str());
#endif
  }
  current_state->ins_queue_full_ = ins_queue_.full();
#ifdef SHOW_ALL
  printf("\tCurrent instruction queue is:\n");
  for (auto entry : ins_queue_) {
    printf("\t\t%s\n", InsToString(entry).c_str());
  }
  printf("\n");
#endif
}

void InstructionUnit::Execute(State *current_state, State *next_state) {
  FetchDecode(current_state, next_state, current_state->input_ins_);
  Issue(current_state, next_state);
}

}  // namespace conless