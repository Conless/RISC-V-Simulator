#ifndef INSTRUCTION_UNIT_H
#define INSTRUCTION_UNIT_H

#include "container/circular_queue.h"

namespace conless {

class Simulator;

class InstructionUnit {
 public:
  void FetchDecode(Simulator &current_state);
  void Issue(Simulator &current_state);

 private:
  circular_queue<InsType> ins_queue_;
};

}

#endif