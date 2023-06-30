#ifndef ARITHMETIC_LOGIC_UNIT_H
#define ARITHMETIC_LOGIC_UNIT_H

#include "common/types.h"
#include "container/circular_queue.h"
#include "unit/base_unit.h"

namespace conless {

struct AluEntry {
  Opcode opcode_;
  int rob_pos_;
  int lhs_, rhs_;
};

class State;
class Bus;

class ArithmeticLogicUnit : public BaseUnit {
 public:
  explicit ArithmeticLogicUnit(Bus *cd_bus) : cd_bus_(cd_bus) {}
  void Flush(State *current_state) override;
  void Execute(State *current_state, State *next_state) override;

 private:
  circular_queue<AluEntry> entries_;
  Bus *cd_bus_;
};

}  // namespace conless

#endif