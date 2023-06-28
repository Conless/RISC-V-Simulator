#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "common/types.h"

namespace conless {

auto SignExtend(uint32_t imm, uint8_t length) -> int;
auto OpcodeToString(Opcode opcode) -> std::string;
void DisplayIns(InsType ins);

}  // namespace conless

#endif