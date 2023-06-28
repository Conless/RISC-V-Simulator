#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "common/types.h"

namespace conless {

auto SignExtend(uint32_t imm, uint8_t length) -> int;
auto OpcodeToString(Opcode opcode) -> std::string;
auto InsToString(InsType ins) -> std::string;

}  // namespace conless

#endif