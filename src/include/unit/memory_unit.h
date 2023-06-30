#ifndef MEMORY_UNIT_H
#define MEMORY_UNIT_H

#include "common/config.h"
#include "common/types.h"
#include "storage/memory.h"
#include "unit/base_unit.h"

namespace conless {

class Bus;
class State;

class MemoryUnit : public BaseUnit {
 public:
  explicit MemoryUnit(Bus *mem_bus) : mem_bus_(mem_bus) {}
  void Init();
  void Flush(State *current_state) override;
  void Execute(State *current_state, State *next_state) override;
  auto FetchWordUnsafe(AddrType pos) -> WordType;

 private:
  Memory<MAX_RAM_SIZE> ram_;
  Bus *mem_bus_;
  int counter_{0};
};
}  // namespace conless

#endif