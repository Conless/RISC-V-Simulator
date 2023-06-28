#ifndef RESERVATION_STATION_H
#define RESERVATION_STATION_H

#include "common/types.h"
#include "common/config.h"
#include "container/array.h"

#include "storage/bus.h"

namespace conless {

struct RssEntry {
  InsType ins_;
  int rob_pos_;
  int num_{0};
  int v1_{0}, v2_{0}; 
  int q1_{-1}, q2_{-1};
};

class State;

class ReservationStation {
 public:
  void Flush(State *current_state);
  void Execute(State *current_state, State *next_state);

 protected:
  void MonitorCdb();
  void ExecuteArith(State *current_state, State *next_state);
  void ExecuteLoadStore(State *current_state, State *next_state);
 private:
  array<RssEntry, MAX_RSS_SIZE> ls_entries_;
  array<RssEntry, MAX_RSS_SIZE> arith_entries_;
  Bus *cd_bus_;
};
}  // namespace conless

#endif