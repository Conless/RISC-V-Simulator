#include "unit/predict_unit.h"
#include <bitset>
#include <cstdint>
#include <cstring>
#include "common/config.h"
#include "common/types.h"

namespace conless {

Predictor::Predictor() { memset(weight_, 0, sizeof(weight_)); }

auto Predictor::GetPredictResult(AddrType PC) -> bool {
  AddrType i = HashPC(PC);
  int y_out = weight_[i][0];
  for (int j = 1; j < HIST_LEN + 1; j++) {
    if (((global_history_ >> (i - 1)) & 1) == 1) {
      y_out += weight_[i][j];
    } else {
      y_out -= weight_[i][j];
    }
  }
  last_y_out_ = abs(y_out);
  return y_out >= 0;
}

void Predictor::PredictFeedBack(AddrType PC, bool result, bool predict) {
  AddrType i = HashPC(PC);
  if (result != predict && !result) {
    failure_count_++;
  } else {
    success_count_++;
  }
  if (result != predict || last_y_out_ <= THETA) {
    if (result) {
      if (weight_[i][0] != THETA) {
        weight_[i][0]++;
      }
    } else {
      if (weight_[i][0] != -THETA) {
        weight_[i][0]--;
      }
    }
    for (int j = 1; j < HIST_LEN + 1; j++) {
      if ((result && (global_history_ >> (j - 1) & 1) == 1) || (!result && (global_history_ >> (j - 1) & 1) == 0)) {
        if (weight_[i][j] != THETA) {
          weight_[i][j]++;
        }
      } else {
        if (weight_[i][j] != -THETA) {
          weight_[i][j]--;
        }
      }
    }
  }
  global_history_ = (global_history_ << 1) | static_cast<int>(result);
}

void Predictor::PrintPredictLog() {
  printf("Success / Failure = %d / %d, Success rate = %.2f\n", success_count_, failure_count_,
         1.0 * success_count_ / (success_count_ + failure_count_));
}

auto Predictor::HashPC(uint32_t PC) -> uint32_t { return PC % PREDICT_UNIT_SIZE; }

}  // namespace conless