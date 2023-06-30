#ifndef MEMORY_H
#define MEMORY_H

#include <exception>
#include "common/types.h"

namespace conless {

class State;

template <size_t RAM_SIZE>
class Memory {
 public:
  void Init();
  auto operator[](const size_t pos) -> ByteType &;

 private:
  ByteType ram_[RAM_SIZE];
};

}  // namespace conless

#endif