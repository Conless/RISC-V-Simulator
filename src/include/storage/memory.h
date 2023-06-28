#ifndef MEMORY_H
#define MEMORY_H

#include "common/config.h"
#include "common/types.h"
#include "storage/bus.h"

namespace conless {

class State;

class Memory {
 public:
  explicit Memory(Bus *mem_bus) : mem_bus_(mem_bus) {}
  void Init();
  void Flush(State *current_state);
  void Execute(State *current_state, State *next_state);
  auto FetchWordUnsafe(AddrType pos) -> WordType;

 private:
  int counter_{0};
  ByteType ram_[MAX_RAM_SIZE];
  Bus *mem_bus_;
};

}  // namespace conless

#endif