#include "simulator.h"
#include "common/types.h"

namespace conless {

Simulator::Simulator() {
  memory_ = new Memory;
  ins_unit_ = new InstructionUnit;
  memory_->Init();

  pc_ = 0;
}

auto Simulator::Run(AddrType pc) -> ReturnType {
  pc_ = pc;
  while (true) {
    ins_unit_->FetchDecode(*this);
  }
  return 0;
}

}  // namespace conless