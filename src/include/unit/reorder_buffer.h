#ifndef REORDER_BUFFER_H
#define REORDER_BUFFER_H

#include "common/types.h"
#include "container/circular_queue.h"

namespace conless {

enum RobState { Issue, Exec, Write, Commit };

struct RobEntry {
  InsType ins_;
  RobState state_;
  AddrType ins_addr_;
  int dest_{-1};
  int value_{0};
};

class State;
class Bus;

class ReorderBuffer {
 public:
  void Flush(State *current_state);
  void Execute(State *current_state, State *next_state);

 protected:
  void MonitorCdb();
  void Commit(State *current_state, State *next_state);

 private:
  circular_queue<RobEntry> entries_;
  Bus *cd_bus_;
};

}  // namespace conless

#endif