#include "simulator.h"

using conless::Simulator;

auto main() -> int {
  Simulator rv32i_cpu;
  std::cout << rv32i_cpu.Run(0x1000) << std::endl;
  return 0;
}