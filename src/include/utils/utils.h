#ifndef UTILS_H
#define UTILS_H

#include "common/types.h"

namespace conless {

auto SignExtend(uint32_t imm, uint8_t length) -> int;

}  // namespace conless

#endif