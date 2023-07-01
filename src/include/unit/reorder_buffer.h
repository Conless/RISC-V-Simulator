#ifndef REORDER_BUFFER_H
#define REORDER_BUFFER_H

#include "common/types.h"
#include "container/circular_queue.h"
#include "unit/base_unit.h"
#include "unit/predict_unit.h"

namespace conless {

enum RobState { Issue, Exec, Write, Side };

struct RobEntry {
  InsType ins_;
  RobState state_;
  int rob_pos_;
  AddrType ins_addr_;
  int dest_{-1};
  int value_{0};
};

class State;
class Bus;

class ReorderBuffer : public BaseUnit {
 public:
  explicit ReorderBuffer(Bus *cd_bus, Predictor *predictor) : cd_bus_(cd_bus), predictor_(predictor) {}
  void Flush(State *current_state) override;
  void Execute(State *current_state, State *next_state) override;

 protected:
  void MonitorCdb(State *current_state);
  void Commit(State *current_state, State *next_state);

 private:
  circular_queue<RobEntry> entries_;
  Bus *cd_bus_;
  Predictor *predictor_;
};

}  // namespace conless

#endif