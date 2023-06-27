#ifndef RESERVATION_STATION_H
#define RESERVATION_STATION_H

#include "common/types.h"
#include "container/circular_queue.h"

namespace conless {
struct RssEntry {
  InsType ins_;
  int rob_pos_;
  int v1_, v2_;
  int q1_, q2_;
  int num_;
  int dest_;
};
class ReservationStation {
 private:
  circular_queue<RssEntry> ls_entries_, arith_entries_;
};
}  // namespace conless

#endif