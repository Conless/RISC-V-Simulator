#include "utils/utils.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include "storage/bus.h"

namespace conless {

auto SignExtend(uint32_t imm, uint8_t length) -> int {
  if ((imm >> length & 1) == 0) {
    return static_cast<int>(imm);
  }
  auto base_num = static_cast<uint32_t>(-1) << length;
  return imm | base_num;
}

auto OpcodeToString(Opcode opcode) -> std::string {
  if (opcode == Opcode::LB) {
    return "LB";
  }
  if (opcode == Opcode::LH) {
    return "LH";
  }
  if (opcode == Opcode::LW) {
    return "LW";
  }
  if (opcode == Opcode::LBU) {
    return "LBU";
  }
  if (opcode == Opcode::LHU) {
    return "LHU";
  }
  if (opcode == Opcode::SB) {
    return "SB";
  }
  if (opcode == Opcode::SH) {
    return "SH";
  }
  if (opcode == Opcode::SW) {
    return "SW";
  }
  if (opcode == Opcode::ADDI) {
    return "ADDI";
  }
  if (opcode == Opcode::SLTI) {
    return "SLTI";
  }
  if (opcode == Opcode::SLTIU) {
    return "SLITU";
  }
  if (opcode == Opcode::XORI) {
    return "XORI";
  }
  if (opcode == Opcode::ORI) {
    return "ORI";
  }
  if (opcode == Opcode::ANDI) {
    return "ANDI";
  }
  if (opcode == Opcode::SLLI) {
    return "SLLI";
  }
  if (opcode == Opcode::SRLI) {
    return "SRLI";
  }
  if (opcode == Opcode::SRAI) {
    return "SRAI";
  }
  if (opcode == Opcode::ADD) {
    return "ADD";
  }
  if (opcode == Opcode::SUB) {
    return "SUB";
  }
  if (opcode == Opcode::SLL) {
    return "SLL";
  }
  if (opcode == Opcode::SLT) {
    return "SLT";
  }
  if (opcode == Opcode::SLTU) {
    return "SLTU";
  }
  if (opcode == Opcode::XOR) {
    return "XOR";
  }
  if (opcode == Opcode::SRL) {
    return "SRL";
  }
  if (opcode == Opcode::SRA) {
    return "SRA";
  }
  if (opcode == Opcode::OR) {
    return "OR";
  }
  if (opcode == Opcode::AND) {
    return "AND";
  }
  if (opcode == Opcode::BEQ) {
    return "BEQ";
  }
  if (opcode == Opcode::BNE) {
    return "BNE";
  }
  if (opcode == Opcode::BLT) {
    return "BLT";
  }
  if (opcode == Opcode::BGE) {
    return "BGE";
  }
  if (opcode == Opcode::BLTU) {
    return "BLTU";
  }
  if (opcode == Opcode::BGEU) {
    return "BGEU";
  }
  if (opcode == Opcode::JAL) {
    return "JAL";
  }
  if (opcode == Opcode::JALR) {
    return "JALR";
  }
  if (opcode == Opcode::AUIPC) {
    return "AUIPC";
  }
  if (opcode == Opcode::LUI) {
    return "LUI";
  }
  throw std::exception();
}

auto IntToHexString(int num) -> std::string {
  std::stringstream ioss;
  std::string str;
  ioss << std::hex << num;
  ioss >> str;
  return str;
}

auto InsToString(InsType ins) -> std::string {
  std::string s = OpcodeToString(ins.opcode_) + ' ';
  if (ins.rs1_ != -1) {
    s += std::to_string(ins.rs1_) + ' ';
  }
  if (ins.rs2_ != -1) {
    s += std::to_string(ins.rs2_) + ' ';
  }
  if (ins.imm_ != -1) {
    s += '(' + IntToHexString(ins.imm_) + ") ";
  }
  if (ins.rd_ != -1) {
    s += "->" + std::to_string(ins.rd_);
  }
  return s;
}

auto RobStateToString(RobState state) -> std::string {
  if (state == RobState::Issue) {
    return "Issue";
  }
  if (state == RobState::Exec) {
    return "Exec";
  }
  if (state == RobState::Side) {
    return "Side";
  }
  if (state == RobState::Write) {
    return "Write";
  }
  throw std::exception();
}

auto BusTypeToString(BusType type) -> std::string {
  if (type == BusType::Executing) {
    return "Executing";
  }
  if (type == BusType::LoadFinish) {
    return "LoadFinish";
  }
  if (type == BusType::LoadRequest) {
    return "LoadRequest";
  }
  if (type == BusType::StoreRequest) {
    return "StoreRequest";
  }
  if (type == BusType::StoreFinish) {
    return "StoreFinish";
  }
  if (type == BusType::SideChannel) {
    return "SideChannel";
  }
  if (type == BusType::WriteBack) {
    return "WriteBack";
  }
  if (type == BusType::CommitReg) {
    return "CommitReg";
  }
  throw std::exception();
}

}  // namespace conless