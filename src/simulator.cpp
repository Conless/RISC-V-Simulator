#include "simulator.h"

#include <algorithm>
#include <random>

#include "common/config.h"
#include "utils/utils.h"

namespace conless {

Simulator::Simulator() {
  cd_bus_ = new Bus(CDB_WIDTH);
  mem_bus_ = new Bus(MEMB_WIDTH);
  predictor_ = new Predictor;
  units_[0] = new MemoryUnit(mem_bus_);
  units_[1] = new InstructionUnit(predictor_);
  units_[2] = new ReorderBuffer(cd_bus_, predictor_);
  units_[3] = new ReservationStation(cd_bus_);
  units_[4] = new ArithmeticLogicUnit(cd_bus_);
  units_[5] = new LoadStoreBuffer(mem_bus_, cd_bus_);
  current_state_ = new State;
  next_state_ = new State;
}

void Simulator::Init(AddrType pc) {
  current_state_ = nullptr;
  next_state_ = new State;
  next_state_->pc_ = pc;
#ifdef SHOW_ALL
  time_t now = time(nullptr);
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
#ifdef SHOW_PC
  if (!current_state_->stall_) {
    printf("Cycle %d, PC %04x\n", clock_, current_state_->pc_);
  }
#endif
#ifdef SHOW_ALL
  Display();
#endif
  for (auto & unit : units_) {
    unit->Flush(current_state_);
  }
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
    if (current_state_->terminate_) {
      printf("Clock cycle = %d\n", clock_);
      predictor_->PrintPredictLog();
      return current_state_->reg_file_.regs_[10].data_ & 255U;
    }
    // std::shuffle(units_, units_ + 6, std::mt19937(std::random_device()()));
    for (auto & unit : units_) {
      unit->Execute(current_state_, next_state_);
    }
  }
  return 0;
}

}  // namespace conless