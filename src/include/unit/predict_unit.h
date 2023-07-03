#ifndef PREDICT_UNIT_H
#define PREDICT_UNIT_H

#include <bitset>
#include <cstdint>

#include "common/config.h"
#include "common/types.h"

namespace conless {

constexpr int HIST_LEN = 59;
constexpr int THETA = 127;

class Predictor {
 private:
  uint64_t global_history_{0};
  uint32_t last_y_out_{0};
  int8_t weight_[PREDICT_UNIT_SIZE][HIST_LEN + 1]{0};

  uint32_t success_count_{0}, failure_count_{0};

 public:
  Predictor();
  auto GetPredictResult(AddrType PC) -> bool;
  void PredictFeedBack(AddrType PC, bool result, bool predict);
  void PrintPredictLog();
  auto HashPC(AddrType PC) -> AddrType;
};

}  // namespace conless
#endif
