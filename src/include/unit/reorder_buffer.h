#ifndef REORDER_BUFFER_H
#define REORDER_BUFFER_H

#include "container/circular_queue.h"

namespace conless {

enum InsState { Issue, Exec, Write, Commit };

struct RobEntry {
  InsType ins_;
  InsState state_;
  int dest_;
  int dest_offset_;
  int value_;
};

class ReorderBuffer {
 public:
  void MonitorCdb();
  void Commit();

//  private:
  circular_queue<RobEntry> entries_;
};

}  // namespace conless

#endif