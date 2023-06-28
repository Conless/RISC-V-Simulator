#ifndef BUS_H
#define BUS_H

#include <utility>

#include "common/config.h"
#include "container/array.h"

namespace conless {

enum BusType { Executing, WriteBack, StoreBack };

struct BusEntry {
  BusType type_;
  int rob_pos_;
  int data_;
};
struct Bus {
  array<BusEntry, BUS_WIDTH> entries_;
};

}  // namespace conless

#endif