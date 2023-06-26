#ifndef MEMORY_H
#define MEMORY_H

#include "common/config.h"
#include "common/types.h"

namespace conless {

class Memory {
 public:
  void Init();
  auto FetchWordUnsafe(AddrType pos) -> WordType;
 private:
  ByteType ram_[MAX_RAM_SIZE];
};

}

#endif