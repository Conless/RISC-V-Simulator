#include "unit/instruction_unit.h"
#include <cstdint>
#include <exception>
#include "common/types.h"
#include "simulator.h"

namespace conless {

auto SignExtend(uint32_t imm, uint8_t length) -> int {
  if ((imm >> length & 1) == 0) {
    return static_cast<int>(imm);
  }
  auto base_num = static_cast<uint32_t>(-1) << length;
  return imm | base_num;
}

void DecodeLoad(WordType ins_reg, InsType &ins) {
  int load_type = ins_reg >> 12 & 0b11;
  bool signed_type = (ins_reg >> 14 & 1) == 0U;
  ins.rs1_ = ins_reg >> 15 & 0b11111;
  ins.rd_ = ins_reg >> 7 & 0b11111;
  ins.imm_ = SignExtend(ins_reg >> 20, 11);
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

void DecodeStore(WordType ins_reg, InsType &ins) {
  int store_type = ins_reg >> 12 & 0b11;
  ins.rs1_ = ins_reg >> 15 & 0b11111;
  ins.rs2_ = ins_reg >> 20 & 0b11111;
  ins.imm_ = SignExtend(((ins_reg >> 25) << 5) + ((ins_reg >> 7) & 0b11111), 11);
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

void DecodeArithI(WordType ins_reg, InsType &ins) {
  int arith_type = ins_reg >> 12 & 0b111;
  ins.rs1_ = ins_reg >> 15 & 0b11111;
  ins.rd_ = ins_reg >> 7 & 0b11111;
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
    bool tag_type = (ins_reg >> 30 & 0b1) == 0U;
    ins.imm_ = ins_reg >> 20 & 0b11111;
    if (arith_type == 0b001) {
      ins.opcode_ = Opcode::SLLI;
    } else if (arith_type == 0b101) {
      ins.opcode_ = tag_type ? Opcode::SRLI : Opcode::SRAI;
    } else {
      throw std::exception();
    }
    return;
  }
  ins.imm_ = SignExtend(ins_reg >> 20, 11);
}

void DecodeArithR(WordType ins_reg, InsType &ins) {
  int arith_type = ins_reg >> 12 & 0b111;
  bool tag_type = (ins_reg >> 30 & 0b1) == 0U;
  ins.rs1_ = ins_reg >> 15 & 0b11111;
  ins.rs2_ = ins_reg >> 20 & 0b11111;
  ins.rd_ = ins_reg >> 7 & 0b11111;
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

void DecodeBranch(WordType ins_reg, InsType &ins) {
  int branch_type = ins_reg >> 12 & 0b111;
  ins.rs1_ = ins_reg >> 15 & 0b11111;
  ins.rs2_ = ins_reg >> 20 & 0b11111;
  ins.imm_ = SignExtend(((ins_reg >> 31 & 0b1) << 12) + ((ins_reg >> 7 & 0b1) << 11) +
                            ((ins_reg >> 25 & 0b111111) << 5) + ((ins_reg >> 8 & 0b1111) << 1),
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

void DecodeOther(WordType ins_reg, InsType &ins) {
  int opcode_num = ins_reg & 0b1111111;
  ins.rd_ = ins_reg >> 7 & 0b11111;
  if (opcode_num == 0b1101111) {
    ins.opcode_ = Opcode::JAL;
    ins.imm_ = SignExtend(((ins_reg >> 31 & 0b1) << 20) + ((ins_reg >> 12 & 0b111111111111) << 12) +
                              ((ins_reg >> 20 & 0b1) << 11) + ((ins_reg >> 21 & 0b1111111111) << 1),
                          20);
  } else if (opcode_num == 0b1100111) {
    ins.opcode_ = Opcode::JALR;
    ins.rs1_ = ins_reg >> 15 & 0b11111;
    ins.imm_ = SignExtend(ins_reg >> 20, 11);
  } else if (opcode_num == 0b0010111) {
    ins.opcode_ = Opcode::AUIPC;
    ins.imm_ = SignExtend((ins_reg >> 12) << 12, 31);
  } else if (opcode_num == 0b0110111) {
    ins.opcode_ = Opcode::LUI;
    ins.imm_ = SignExtend((ins_reg >> 12) << 12, 31);
  } else {
    throw std::exception();
  }
}

void Decode(WordType ins_reg, InsType &ins) {
  int opcode_num = ins_reg & 0b1111111;
  if (opcode_num == 0b0000011) {
    ins.opcode_type_ = OpcodeType::LOAD;
    DecodeLoad(ins_reg, ins);
    return;
  }
  if (opcode_num == 0b0100011) {
    ins.opcode_type_ = OpcodeType::STORE;
    DecodeStore(ins_reg, ins);
    return;
  }
  if (opcode_num == 0b0010011) {
    ins.opcode_type_ = OpcodeType::ARITHI;
    DecodeArithI(ins_reg, ins);
    return;
  }
  if (opcode_num == 0b0110011) {
    ins.opcode_type_ = OpcodeType::ARITHR;
    DecodeArithR(ins_reg, ins);
    return;
  }
  if (opcode_num == 0b1100011) {
    ins.opcode_type_ = OpcodeType::BRANCH;
    DecodeBranch(ins_reg, ins);
    return;
  }
  ins.opcode_type_ = OpcodeType::OTHER;
  DecodeOther(ins_reg, ins);
}

void InstructionUnit::FetchDecode(Simulator &current_state) {
  WordType ins_reg = current_state.memory_->FetchWordUnsafe(current_state.pc_);
  InsType ins;
  if (ins_queue_.full() || current_state.stall_) {
    return;
  }
  if (ins_reg == 0x0ff00513) {
    // TODO(conless): fix this instruction
    return;
  }
  Decode(ins_reg, ins);
  ins_queue_.push(ins);
  current_state.pc_ += 4;
}
}  // namespace conless