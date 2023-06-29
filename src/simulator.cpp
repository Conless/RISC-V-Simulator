#include "simulator.h"
#include "common/config.h"
#include "storage/bus.h"
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
#ifdef SHOW_ALL
  time_t now = time(nullptr);

  // 把 now 转换为字符串形式
  char *dt = ctime(&now);

  std::cout << "Debugging log at " << dt << std ::endl;
#endif
}

void Simulator::Display() {
  printf("Clock cycle %d:\n", clock_);
  printf("\tCurrent pc: %x, Input instruction is %x\n", current_state_->pc_, current_state_->input_ins_);
  printf("\tStall status: %d, Clean status: %d\n", current_state_->stall_, current_state_->clean_);  // NOLINT
  printf("\tRegister file:\n");
  for (int i = 0; i < REG_FILE_SIZE; i++) {
    auto &reg = current_state_->reg_file_.regs_[i];
    if (reg.data_ == 0 && reg.dependency_ == -1) {
      continue;
    }
    printf("\t\tRegs[%d]: %x (%d)\n", i, reg.data_, reg.dependency_);
  }
  printf("\n");
  printf("\tCommon data bus:\n");
  for (auto entry : cd_bus_->entries_) {
    printf("\t\t%-15s @%-9d (%x)\n", BusTypeToString(entry.second.type_).c_str(), entry.second.pos_,
           entry.second.data_);
  }
  printf("\tMemory bus:\n");
  for (auto entry : mem_bus_->entries_) {
    printf("\t\t%-15s 0x%-8x (%x)\n", BusTypeToString(entry.second.type_).c_str(), entry.second.pos_,
           entry.second.data_);
  }
  printf("\tStore request: ");
  if (current_state_->st_req_.first) {
    printf("%x @%d\n", current_state_->st_req_.second.second, current_state_->st_req_.second.first);
  } else {
    printf("empty\n");
  }
  printf("\n");
}

void Simulator::Flush() {
  // if (clock_ > 15000) {
  //   exit(0);
  // }
  clock_++;
  delete current_state_;
  current_state_ = next_state_;
  current_state_->reg_file_.regs_[0] = {0, -1};

  if (current_state_->clean_) {
    if (mem_bus_->entries_.busy(0) && mem_bus_->entries_[0].type_ != BusType::StoreRequest) {
      mem_bus_->entries_.clean();
    }
    cd_bus_->entries_.clean();

    current_state_->stall_ = current_state_->ins_reg_.first = false;
    current_state_->rob_entry_.first = current_state_->rss_entry_.first = false;
    current_state_->alu_entry_.first = current_state_->lsb_entry_.first = false;

    for (auto &reg : current_state_->reg_file_.regs_) {
      reg.dependency_ = -1;
    }
  }

  if (!current_state_->stall_) {
    current_state_->input_ins_ = memory_->FetchWordUnsafe(current_state_->pc_);
  }
#ifdef SHOW_PC
  if (!current_state_->stall_) {
    printf("Cycle %d, PC %x\n", clock_, current_state_->pc_);
  }
#endif
#ifdef SHOW_ALL
  Display();
#endif
  memory_->Flush(current_state_);
  ins_unit_->Flush(current_state_);
  ls_buffer_->Flush(current_state_);
  ro_buffer_->Flush(current_state_);
  rs_station_->Flush(current_state_);
  arith_logic_unit_->Flush(current_state_);
  cd_bus_->entries_.clean();

#ifdef SHOW_ALL
  printf("\tRob tail: %d\n", current_state_->rob_tail_);
  printf("\n\n");
#endif
  next_state_ = new State;
  next_state_->pc_ = current_state_->pc_;
  next_state_->stall_ = current_state_->stall_;
  next_state_->reg_file_ = current_state_->reg_file_;
  next_state_->st_req_ = current_state_->st_req_;
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