#include "simulator/simulator.h"
#include "common/types.h"

namespace conless {


Simulator::Simulator() {
  memory_ = new Memory;
  
  memory_->Init();  
}

}  // namespace conless