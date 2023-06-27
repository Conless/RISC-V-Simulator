#ifndef ARITHMETIC_LOGIC_UNIT_H
#define ARITHMETIC_LOGIC_UNIT_H

#include "common/types.h"
#include "container/circular_queue.h"

namespace conless {

struct AluEntry {
  Opcode opcode_;
  int rob_pos_;
  int lhs_, rhs_;
};

class State;
class Bus;

class ArithmeticLogicUnit {
 public:
  void Flush(State *current_state);
  void Execute();

 private:
  circular_queue<AluEntry> entries_;
  Bus *cdb_;
};

}  // namespace conless

#endif