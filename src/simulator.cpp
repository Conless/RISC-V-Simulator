#include "simulator.h"
#include "common/config.h"
#include "utils/utils.h"

namespace conless {

Simulator::Simulator() {
  cd_bus_ = new Bus;
  mem_bus_ = new Bus;
  memory_ = new Memory(mem_bus_);
  ins_unit_ = new InstructionUnit;
  ro_buffer_ = new ReorderBuffer(cd_bus_);
  rs_station_ = new ReservationStation(cd_bus_);
  arith_logic_unit_ = new ArithmeticLogicUnit(cd_bus_);
  ls_buffer_ = new LoadStoreBuffer(mem_bus_, cd_bus_);
  current_state_ = new State;
  next_state_ = new State;
}

void Simulator::Init(AddrType pc) {
  memory_->Init();
  current_state_ = nullptr;
  next_state_ = new State;
  next_state_->pc_ = pc;
#ifdef DEBUG
  time_t now = time(nullptr);

  // 把 now 转换为字符串形式
  char *dt = ctime(&now);

  std::cout << "Debugging log at " << dt << std ::endl;
#endif
}

void Simulator::Display() {
  printf("Clock cycle %d:\n\tcurrent pc: %x, Input instruction is %x\n\tStall status: %d, Clean status: %d\n", clock_,
         current_state_->pc_, current_state_->input_ins_, current_state_->stall_, current_state_->clean_);  // NOLINT
  printf("\tRegister file:\n");
  for (int i = 0; i < REG_FILE_SIZE; i++) {
    auto &reg = current_state_->reg_file_.regs_[i];
    if (reg.data_ == 0 && reg.dependency_ == -1) {
      continue;
    }
    printf("\t\tRegs[%d]: %d (%d)\n", i, reg.data_, reg.dependency_);
  }
  printf("\n");
  printf("\tCommon data bus:\n");
  for (auto entry : cd_bus_->entries_) {
    printf("\t\t%-15s @%-9d (%d)\n", BusTypeToString(entry.second.type_).c_str(), entry.second.pos_, entry.second.data_);
  }
  printf("\tMemory bus:\n");
  for (auto entry : mem_bus_->entries_) {
    printf("\t\t%-15s 0x%-8x (%d)\n", BusTypeToString(entry.second.type_).c_str(), entry.second.pos_, entry.second.data_);
  }
  printf("\n");
}

void Simulator::Flush() {
  clock_++;
  delete current_state_;
  current_state_ = next_state_;
  if (!current_state_->stall_) {
    current_state_->input_ins_ = memory_->FetchWordUnsafe(current_state_->pc_);
  }
#ifdef DEBUG
  if (clock_ > 1e3) {
    exit(0);
  }
  Display();
#endif
  memory_->Flush(current_state_);
  ins_unit_->Flush(current_state_);
  ro_buffer_->Flush(current_state_);
  rs_station_->Flush(current_state_);
  arith_logic_unit_->Flush(current_state_);
  ls_buffer_->Flush(current_state_);
  cd_bus_->entries_.clean();

#ifdef DEBUG
  printf("\n\n");
#endif
  next_state_ = new State;
  next_state_->stall_ = current_state_->stall_;
  next_state_->clean_ = current_state_->clean_;
  next_state_->reg_file_ = current_state_->reg_file_;
}

auto Simulator::Run() -> ReturnType {
  while (true) {
    Flush();
    memory_->Execute(current_state_, next_state_);
    ins_unit_->Execute(current_state_, next_state_);
    ro_buffer_->Execute(current_state_, next_state_);
    rs_station_->Execute(current_state_, next_state_);
    arith_logic_unit_->Execute(current_state_, next_state_);
    ls_buffer_->Execute(current_state_, next_state_);
  }
  return 0;
}

}  // namespace conless