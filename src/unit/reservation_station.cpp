#include "unit/reservation_station.h"
#include <exception>
#include "common/config.h"
#include "common/types.h"
#include "simulator.h"
#include "storage/bus.h"
#include "unit/arithmetic_logic_unit.h"
#include "utils/utils.h"

namespace conless {

void ReservationStation::Flush(State *current_state) {
  if (current_state->clean_) {
    ls_entries_.clean();
    arith_entries_.clean();
    current_state->ls_full_ = ls_entries_.full();
    current_state->arith_full_ = arith_entries_.full();
    return;
  }
  if (current_state->rss_entry_.first) {
    if (current_state->rss_entry_.second.ins_.opcode_type_ == OpcodeType::STORE ||
        current_state->rss_entry_.second.ins_.opcode_type_ == OpcodeType::LOAD) {
      int space = ls_entries_.space();
      if (space == -1) {
        throw std::exception();
      }
      ls_entries_[space] = current_state->rss_entry_.second;
    } else {
      int space = arith_entries_.space();
      if (space == -1) {
        throw std::exception();
      }
      arith_entries_[space] = current_state->rss_entry_.second;
    }
#ifdef SHOW_ALL
    printf("\tReservation station receives @%d: %s\n", current_state->rss_entry_.second.rob_pos_,
           InsToString(current_state->rss_entry_.second.ins_).c_str());
#endif
  }
  current_state->ls_full_ = ls_entries_.full();
  current_state->arith_full_ = arith_entries_.full();
  MonitorCdb();
#ifdef SHOW_ALL
  printf("\tCurrent reservation station is:\n");
  for (auto entry : arith_entries_) {
    printf("\t\t@%-7d  %-20s   %-7x %-7x %-7d %-7d\n", entry.second.rob_pos_, InsToString(entry.second.ins_).c_str(),
           entry.second.v1_, entry.second.v2_, entry.second.q1_, entry.second.q2_);
  }
  for (auto entry : ls_entries_) {
    printf("\t\t@%-7d  %-20s   %-7x %-7x %-7d %-7d\n", entry.second.rob_pos_, InsToString(entry.second.ins_).c_str(),
           entry.second.v1_, entry.second.v2_, entry.second.q1_, entry.second.q2_);
  }
  printf("\n");
#endif
}

void ReservationStation::Execute(State *current_state, State *next_state) {
  ExecuteArith(current_state, next_state);
  ExecuteLoadStore(current_state, next_state);
}

void ReservationStation::MonitorCdb() {
  std::pair<bool, int> updated_data[MAX_ROB_SIZE]{{false, 0}};
  for (auto bus_entry : cd_bus_->entries_) {
    if (bus_entry.second.type_ != BusType::WriteBack && bus_entry.second.type_ != BusType::CommitReg) {
      continue;
    }
    updated_data[bus_entry.second.pos_ % MAX_QUEUE_SIZE] = {true, bus_entry.second.data_};
  }
  for (auto &entry : arith_entries_) {
    if (entry.second.q1_ != -1 && updated_data[entry.second.q1_ % MAX_QUEUE_SIZE].first) {
      entry.second.v1_ = updated_data[entry.second.q1_ % MAX_QUEUE_SIZE].second;
      entry.second.q1_ = -1;
    }
    if (entry.second.q2_ != -1 && updated_data[entry.second.q2_ % MAX_QUEUE_SIZE].first) {
      entry.second.v2_ = updated_data[entry.second.q2_ % MAX_QUEUE_SIZE].second;
      entry.second.q2_ = -1;
    }
  }
  for (auto &entry : ls_entries_) {
    if (entry.second.q1_ != -1 && updated_data[entry.second.q1_ % MAX_QUEUE_SIZE].first) {
      entry.second.v1_ = updated_data[entry.second.q1_ % MAX_QUEUE_SIZE].second;
      entry.second.q1_ = -1;
    }
    if (entry.second.q2_ != -1 && updated_data[entry.second.q2_ % MAX_QUEUE_SIZE].first) {
      entry.second.v2_ = updated_data[entry.second.q2_ % MAX_QUEUE_SIZE].second;
      entry.second.q2_ = -1;
    }
  }
  // for (auto bus_entry : cd_bus_->entries_) {
  //   if (bus_entry.second.type_ != BusType::WriteBack && bus_entry.second.type_ != BusType::CommitReg) {
  //     continue;
  //   }
  //   std::pair<int, int> dep_data = {bus_entry.second.pos_ % MAX_QUEUE_SIZE, bus_entry.second.data_};
  //   for (auto &entry : arith_entries_) {
  //     if (entry.second.q1_ % MAX_QUEUE_SIZE == dep_data.first) {
  //       entry.second.v1_ = dep_data.second;
  //       entry.second.q1_ = -1;
  //     }
  //     if (entry.second.q2_ % MAX_QUEUE_SIZE == dep_data.first) {
  //       entry.second.v2_ = dep_data.second;
  //       entry.second.q2_ = -1;
  //     }
  //   }
  //   for (auto &entry : ls_entries_) {
  //     if (entry.second.q1_ % MAX_QUEUE_SIZE == dep_data.first) {
  //       entry.second.v1_ = dep_data.second;
  //       entry.second.q1_ = -1;
  //     }
  //     if (entry.second.q2_ % MAX_QUEUE_SIZE == dep_data.first) {
  //       entry.second.v2_ = dep_data.second;
  //       entry.second.q2_ = -1;
  //     }
  //   }
  // }
}

void ReservationStation::ExecuteArith(State *current_state, State *next_state) {
  for (auto &entry : arith_entries_) {
    if (!entry.first || entry.second.q1_ != -1 || entry.second.q2_ != -1) {
      continue;
    }
    AluEntry alu_entry{entry.second.ins_.opcode_, entry.second.rob_pos_};
    BusEntry bus_entry{BusType::Executing, alu_entry.rob_pos_, 0};
    if (entry.second.ins_.opcode_type_ == OpcodeType::ARITHI || entry.second.ins_.opcode_ == Opcode::JALR) {
      alu_entry.lhs_ = entry.second.v1_;
      alu_entry.rhs_ = entry.second.num_;
    } else if (entry.second.ins_.opcode_type_ == OpcodeType::BRANCH ||
               entry.second.ins_.opcode_type_ == OpcodeType::ARITHR) {
      alu_entry.lhs_ = entry.second.v1_;
      alu_entry.rhs_ = entry.second.v2_;
    } else {
      throw std::exception();
    }
    next_state->alu_entry_ = {true, alu_entry};
    int space = cd_bus_->entries_.space();
    if (space == -1) {
      throw std::exception();
    }
    cd_bus_->entries_[space] = bus_entry;
    entry.first = false;
    return;
  }
}

void ReservationStation::ExecuteLoadStore(State *current_state, State *next_state) {
  for (int i = 0; i < ls_entries_.capacity(); i++) {
    if (!ls_entries_.busy(i)) {
      return;
    }
    auto &entry = ls_entries_[i];
    if (entry.q1_ != -1) {
      if (entry.ins_.opcode_type_ == OpcodeType::STORE) {
        return;
      }
      continue;
    }
    if (entry.ins_.opcode_type_ == OpcodeType::LOAD && current_state->load_full_) {
      continue;
    }
    if (entry.ins_.opcode_type_ == OpcodeType::STORE && current_state->store_full_) {
      return;
    }
    LsbEntry lsb_entry{entry.ins_.opcode_type_, static_cast<AddrType>(entry.v1_ + entry.num_), 0, entry.rob_pos_,
                       entry.ins_.opcode_ != Opcode::LBU && entry.ins_.opcode_ != Opcode::LHU};
    BusEntry bus_entry{entry.ins_.opcode_type_ == OpcodeType::STORE ? BusType::WriteBack : BusType::Executing,
                       entry.rob_pos_, 0};
    if (entry.ins_.opcode_ == Opcode::LB || entry.ins_.opcode_ == Opcode::LBU || entry.ins_.opcode_ == Opcode::SB) {
      lsb_entry.length_ = 1;
    } else if (entry.ins_.opcode_ == Opcode::LH || entry.ins_.opcode_ == Opcode::LHU ||
               entry.ins_.opcode_ == Opcode::SH) {
      lsb_entry.length_ = 2;
    } else if (entry.ins_.opcode_ == Opcode::LW || entry.ins_.opcode_ == Opcode::SW) {
      lsb_entry.length_ = 4;
    }
    next_state->lsb_entry_ = {true, lsb_entry};
    int space = cd_bus_->entries_.space();
    if (space == -1) {
      throw std::exception();
    }
    cd_bus_->entries_[space] = bus_entry;
    ls_entries_.erase_move(i);
    return;
  }
}

}  // namespace conless