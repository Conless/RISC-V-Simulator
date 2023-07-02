#ifndef PREDICT_UNIT_H
#define PREDICT_UNIT_H

#include <bitset>

#include "common/config.h"
#include "common/types.h"

namespace conless {

struct Counter {
  std::bitset<2> data_{0b10};
  auto operator++() -> Counter & {
    if (data_ == 0b00) {
      data_ = 0b01;
    } else if (data_ == 0b01) {
      data_ = 0b10;
    } else {
      data_ = 0b11;
    }
    return *this;
  }
  auto operator--() -> Counter & {
    if (data_ == 0b11) {
      data_ = 0b10;
    } else if (data_ == 0b10) {
      data_ = 0b01;
    } else {
      data_ = 0b00;
    }
    return *this;
  }
};

class Predictor {
 public:
  auto GetPredictResult(AddrType pc) -> bool {
    pc = GetPcHash(pc);
    return counters_[pc].data_[1];
  }
  void PredictFeedBack(AddrType pc, bool predict_result, bool real_result) {
    pc = GetPcHash(pc);
    if (real_result) {
      ++counters_[pc];
    } else {
      --counters_[pc];
    }
    if (real_result == predict_result) {
      ++success_count_;
    } else {
      ++failure_count_;
    }
  }
  void PrintPredictLog() {
    printf("Success / Failure = %d / %d, Success Rate = %.2f\n", success_count_, failure_count_,
           1.0 * success_count_ / (success_count_ + failure_count_));
  }

 private:
  auto GetPcHash(AddrType pc) -> AddrType { return pc % PREDICT_UNIT_SIZE; }

  Counter counters_[PREDICT_UNIT_SIZE];
  int success_count_{0}, failure_count_{0};
};

}  // namespace conless

#endif