#ifndef BUS_H
#define BUS_H

#include <utility>

#include "common/config.h"
#include "container/array.h"

namespace conless {

enum BusType {
  Executing,
  WriteBack,
  SideChannel,
  LoadFinish,
  StoreFinish,
  LoadRequest,
  StoreRequest
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