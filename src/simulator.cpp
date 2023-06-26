#include "simulator.h"
#include "common/types.h"

namespace conless {

Simulator::Simulator() {
  memory_ = new Memory;
  ins_unit_ = new InstructionUnit;
  memory_->Init();

  pc_ = 0;
}

void Simulator::FetchDecode() {
  if (stall_) {
    return;
  }
  WordType ins_reg = memory_->FetchWordUnsafe(pc_);
  ins_unit_->FetchDecode(*this, ins_reg);
}

auto Simulator::Run(AddrType pc) -> ReturnType {
  pc_ = pc;
  while (true) {
    FetchDecode();
  }
  return 0;
}

}  // namespace conless