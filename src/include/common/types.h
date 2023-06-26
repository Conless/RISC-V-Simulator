#ifndef TYPES_H
#define TYPES_H

#include <cstddef>
#include <cstdint>

namespace conless {

using AddrType = uint32_t;
using DataType = uint32_t;
using WordType = uint32_t;
using ReturnType = uint8_t;
using ByteType = uint8_t;
using OpcodeNumType = uint8_t;

enum OpcodeType { LOAD, STORE, ARITHI, ARITHR, BRANCH, OTHER};
enum Opcode {
  // LOAD
  LB,   // load byte
  LH,   // load halfword
  LW,   // load word
  LBU,  // load byte unsigned
  LHU,  // load halfword unsigned
  // STORE
  SB,  // store byte
  SH,  // store halfword
  SW,  // store word
  // ARITHI
  ADDI,   // add immediate
  SLTI,   // set if less than immediate
  SLTIU,  // set if less than immediate unsigned
  XORI,   // xor immediate
  ORI,    // or immediate
  ANDI,   // and immediate
  SLLI,   // shift left logical immediate
  SRLI,   // shift right logical immediate
  SRAI,   // shift right arithmetic immediate
  // ARITHR
  ADD,   // add
  SUB,   // substract
  SLL,   // shift left logical
  SLT,   // set if less than
  SLTU,  // set if less than unsigned
  XOR,   // xor
  SRL,   // shift right logical
  SRA,   // shift right arithmetic
  OR,    // or
  AND,   // and
  // BRANCH
  BEQ,   // branch if equal
  BNE,   // branch if not equal
  BLT,   // branch if less than
  BGE,   // branch if greater than or equal
  BLTU,  // branch if less than unsigned
  BGEU,  // branch if greater than or equal unsigned
  // OTHER
  JAL,    // jump and link
  JALR,   // jump and link register
  AUIPC,  // add upper immediate to pc
  LUI,    // load upper immediate
};

struct InsType {
  OpcodeType opcode_type_;
  Opcode opcode_;
  int rs1_, rs2_;
  int imm_;
};

}  // namespace conless

#endif