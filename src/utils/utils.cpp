#include "utils/utils.h"

namespace conless {
auto SignExtend(uint32_t imm, uint8_t length) -> int {
  if ((imm >> length & 1) == 0) {
    return static_cast<int>(imm);
  }
  auto base_num = static_cast<uint32_t>(-1) << length;
  return imm | base_num;
}
}  // namespace conless