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
  CommitReg,
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
  explicit Bus(int width) : entries_(width) {}
  array<BusEntry> entries_;
};

}  // namespace conless

#endif