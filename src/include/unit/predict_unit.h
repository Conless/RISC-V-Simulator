#ifndef PREDICT_UNIT_H
#define PREDICT_UNIT_H

#include <bitset>
#include <map>

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
    // return true;
    return counters_[pc].data_[1];
  }
  void PredictFeedBack(AddrType pc, bool predict_result, bool real_result) {
    if (real_result) {
      ++counters_[pc];
    } else {
      --counters_[pc];
    }
    if (real_result == predict_result) {
      ++predict_log_.first;
    } else {
      ++predict_log_.second;
    }
  }
  void PrintPredictLog() {
    printf("Success / Failure = %d / %d, Success Rate = %.2f\n", predict_log_.first, predict_log_.second,
           1.0 * predict_log_.first / (predict_log_.first + predict_log_.second));
  }

 private:
  std::map<AddrType, Counter> counters_;
  std::pair<int, int> predict_log_;
};

}  // namespace conless

#endif