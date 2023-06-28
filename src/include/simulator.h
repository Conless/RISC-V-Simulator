#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <queue>

#include "common/config.h"
#include "common/types.h"
#include "container/circular_queue.h"
#include "storage/memory.h"

#include "unit/arithmetic_logic_unit.h"
#include "unit/instruction_unit.h"
#include "unit/register_file.h"
#include "unit/reorder_buffer.h"
#include "unit/reservation_station.h"
#include "unit/load_store_buffer.h"

namespace conless {

struct State {
  AddrType pc_{0};
  WordType input_ins_;
  bool stall_{false}, clean_{false};

  bool ins_queue_full_{false}; // flush by iu at the beginning of this cycle
  std::pair<bool, InsType> ins_reg_{false, InsType{}}; // edit by FetchDecode in last cycle

  bool rob_full_{false}; // flush by rob at the beginning of this cycle
  int rob_tail_{0}; 
  std::pair<bool, RobEntry> rob_entry_{false, RobEntry()}; // edit by Issue in last cycle

  bool arith_full_{false}, ls_full_{false}; // flush by rss at the beginning of this cycle
  std::pair<bool, RssEntry> rss_entry_{false, RssEntry()};  // edit by Execute in last cycle

  std::pair<bool, AluEntry> alu_entry_{false, AluEntry()};

  bool load_full_{false}, st_full_{false};  // flush by lsb at the beginning of this cycle
  circular_queue<int, 4> st_req_;
  std::pair<bool, LsbEntry> lsb_entry_{false, LsbEntry()}; // edit by LoadStore in last cycle

  RegisterFile reg_file_; // edit by Commit in last cycle
};

class Simulator {
  friend class InstructionUnit;

 public:
  Simulator();
  void Init(AddrType pc);
  auto Run() -> ReturnType;

 protected:
  void Display();
  void Flush();

 private:
  int clock_{0};
  Bus *cd_bus_;
  Bus *mem_bus_;
  Memory *memory_;
  InstructionUnit *ins_unit_;
  ReorderBuffer *ro_buffer_;
  ReservationStation *rs_station_;
  ArithmeticLogicUnit *arith_logic_unit_;
  LoadStoreBuffer *ls_buffer_;
  State *current_state_, *next_state_;
};

}  // namespace conless

#endif