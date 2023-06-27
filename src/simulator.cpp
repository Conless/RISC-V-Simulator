#include "simulator.h"
#include "common/types.h"

namespace conless {

Simulator::Simulator() {
  memory_ = new Memory;
  ins_unit_ = new InstructionUnit;
  ro_buffer_ = new ReorderBuffer;
  current_state_ = new State;
  next_state_ = new State;
}

void Simulator::Init(AddrType pc) {
  memory_->Init();
  current_state_ = nullptr;
  next_state_ = new State;
  next_state_->pc_ = pc;
}

void Simulator::Flush() {
  delete current_state_;
  current_state_ = next_state_;

  ins_unit_->Flush(current_state_);
  ro_buffer_->Flush(current_state_);
  rs_station_->Flush(current_state_);
  ls_buffer_->Flush(current_state_);

  next_state_ = new State;
  next_state_->stall_ = current_state_->stall_;
  next_state_->reg_file_ = current_state_->reg_file_;
}

void Simulator::FetchDecode() {
  if (current_state_->stall_) {
    return;
  }
  WordType input_ins = memory_->FetchWordUnsafe(current_state_->pc_);
  ins_unit_->FetchDecode(current_state_, input_ins);
}

auto Simulator::Run() -> ReturnType {
  while (true) {
    FetchDecode();
  }
  return 0;
}

}  // namespace conless