#ifndef BUS_H
#define BUS_H

#include <utility>

#include "common/config.h"
#include "container/array.h"

namespace conless {

enum BusType {
  Executing = 0,
  WriteBack = 1,
  SideChannel = 2,
  LoadFinish = 0,
  StoreFinish = 1,
  LoadRequest = 2,
  StoreRequest = 3
};

struct BusEntry {
  BusType type_;
  int pos_;
  int data_;
};
struct Bus {
  array<BusEntry, BUS_WIDTH> entries_;
};

}  // namespace conless

#endif