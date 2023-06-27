#ifndef INSTRUCTION_UNIT_H
#define INSTRUCTION_UNIT_H

#include "common/types.h"
#include "container/circular_queue.h"
#include "unit/predict_unit.h"

namespace conless {

class State;

class InstructionUnit {
 public:
  void FetchDecode(State *current_state, State *next_state, WordType input_ins);
  void Issue(State *current_state, State *next_state);
  void Flush(State *current_state);

 private:
  PredictUnit predictor_;
  circular_queue<InsType> ins_queue_;
};

}  // namespace conless

#endif