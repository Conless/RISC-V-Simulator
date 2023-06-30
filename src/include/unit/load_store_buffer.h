#ifndef LOAD_STORE_BUFFER_H
#define LOAD_STORE_BUFFER_H

#include <cstring>
#include "common/types.h"
#include "container/circular_queue.h"
#include "storage/bus.h"
#include "unit/base_unit.h"

namespace conless {

struct LsbEntry {
  OpcodeType type_;
  AddrType start_addr_;
  int length_;
  int rob_pos_;
  bool signed_tag_;
};

class State;

class LoadStoreBuffer : public BaseUnit {
 public:
  LoadStoreBuffer(Bus *mem_bus, Bus *cd_bus) : mem_bus_(mem_bus), cd_bus_(cd_bus) {}
  void Flush(State *current_state);
  void Execute(State *current_state, State *next_state);

 protected:
  void MonitorMemb();

 private:
  circular_queue<LsbEntry> load_buffer_, store_buffer_;
  circular_queue<BusEntry> temp_bus_entries_;
  Bus *mem_bus_;
  Bus *cd_bus_;
};

}  // namespace conless

#endif