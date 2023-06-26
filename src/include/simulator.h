#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>

#include "common/config.h"
#include "common/types.h"
#include "storage/memory.h"
#include "unit/instruction_unit.h"
#include "unit/reorder_buffer.h"

namespace conless {

struct State {
  AddrType pc_;
  bool ins_queue_full_, rob_full_, rss_ls_full_, rss_arith_full_;
  int rob_tail_;
  bool stall_;
  RegisterFile *reg_file_;
}

class Simulator {
  friend class InstructionUnit;

 public:
  Simulator();
  void Init();
  auto Run(AddrType pc) -> ReturnType;

 protected:
  void FetchDecode();

 private:
  Memory *memory_;
  InstructionUnit *ins_unit_;
  ReorderBuffer *ro_buffer_;
};

}  // namespace conless

#endif