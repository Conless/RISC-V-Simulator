#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "common/types.h"
#include "unit/reorder_buffer.h"
#include "storage/bus.h"

namespace conless {

auto SignExtend(uint32_t imm, uint8_t length) -> int;
auto OpcodeToString(Opcode opcode) -> std::string;
auto InsToString(InsType ins) -> std::string;

auto RobStateToString(RobState state) -> std::string;
auto BusTypeToString(BusType type) -> std::string;

}  // namespace conless

#endif