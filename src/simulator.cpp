#include "simulator.h"

namespace conless {

Simulator::Simulator() {
  cd_bus_ = new Bus;
  mem_bus_ = new Bus;
  memory_ = new Memory(mem_bus_);
  ins_unit_ = new InstructionUnit;
  ro_buffer_ = new ReorderBuffer(cd_bus_);
  rs_station_ = new ReservationStation(cd_bus_);
  arith_logic_unit_ = new ArithmeticLogicUnit(cd_bus_);
  ls_buffer_ = new LoadStoreBuffer(cd_bus_, mem_bus_);
  current_state_ = new State;
  next_state_ = new State;
}

void Simulator::Init(AddrType pc) {
  memory_->Init();
  current_state_ = nullptr;
  next_state_ = new State;
  next_state_->pc_ = pc;
}

void Simulator::Display() {
  printf(
      "Clock cycle %d:\n\tcurrent pc: %x, Input instruction is %x\n\tStall status: %d, Clean status: %d\n",
      clock_, current_state_->pc_, current_state_->input_ins_, current_state_->stall_, current_state_->clean_); // NOLINT
}

void Simulator::Flush() {
  clock_++;
  delete current_state_;
  current_state_ = next_state_;
  if (!current_state_->stall_) {
    current_state_->input_ins_ = memory_->FetchWordUnsafe(current_state_->pc_);
  }
#ifdef DEBUG
  Display();
#endif
  memory_->Flush(current_state_);
  ins_unit_->Flush(current_state_);
  ro_buffer_->Flush(current_state_);
  rs_station_->Flush(current_state_);
  arith_logic_unit_->Flush(current_state_);
  ls_buffer_->Flush(current_state_);

#ifdef DEBUG
  printf("\n\n");
#endif
  next_state_ = new State;
  next_state_->stall_ = current_state_->stall_;
  next_state_->clean_ = current_state_->stall_;
  next_state_->reg_file_ = current_state_->reg_file_;
}

auto Simulator::Run() -> ReturnType {
  while (true) {
    Flush();
    ins_unit_->Execute(current_state_, next_state_);
    ro_buffer_->Execute(current_state_, next_state_);
    rs_station_->Execute(current_state_, next_state_);
    arith_logic_unit_->Execute(current_state_, next_state_);
    ls_buffer_->Execute(current_state_, next_state_);
  }
  return 0;
}

}  // namespace conless