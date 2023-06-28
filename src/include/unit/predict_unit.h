#ifndef PREDICT_UNIT_H
#define PREDICT_UNIT_H

#include "common/types.h"

namespace conless {

class PredictUnit {
 public:
  auto GetPredictResult(AddrType pc) -> bool { return true; }
};

}

#endif