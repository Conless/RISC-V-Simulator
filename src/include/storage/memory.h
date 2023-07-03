#ifndef MEMORY_H
#define MEMORY_H

#include <exception>
#include "common/config.h"
#include "common/types.h"

namespace conless {

class State;

class Memory {
 public:
  void Init();
  auto operator[](const size_t pos) -> ByteType &;

 private:
  ByteType ram_[MAX_RAM_SIZE];
};

}  // namespace conless

#endif