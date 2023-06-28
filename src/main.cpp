#include "simulator.h"

using conless::Simulator;

auto main() -> int {
  Simulator rv32i_cpu;
  rv32i_cpu.Init(0x0);
  std::cout << rv32i_cpu.Run() << std::endl;
  return 0;
}