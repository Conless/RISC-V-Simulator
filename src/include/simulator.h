#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include "common/config.h"
#include "common/types.h"
#include "storage/memory.h"
#include "unit/instruction_unit.h"

namespace conless {

struct State {
  
};

class Simulator {
  friend class InstructionUnit;

 public:
  Simulator();
  auto Run(AddrType pc) -> ReturnType;

 private:
  AddrType pc_;
  Memory *memory_;
  InstructionUnit *ins_unit_;
};

}  // namespace conless

#endif