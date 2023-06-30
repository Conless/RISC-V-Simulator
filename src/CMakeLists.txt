set(PROJECT_NAME ${CMAKE_PROJECT_NAME})

set(UNIT_SOURCE_CPPS
  unit/arithmetic_logic_unit.cpp
  unit/instruction_unit.cpp
  unit/load_store_buffer.cpp
  unit/memory_unit.cpp
  unit/reorder_buffer.cpp
  unit/reservation_station.cpp
)

set(STORAGE_SOURCE_CPPS
  storage/memory.cpp
  storage/bus.cpp
)

set(UTILS_SOURCE_CPPS
  utils/utils.cpp
)

add_library(simulator ${UNIT_SOURCE_CPPS} ${STORAGE_SOURCE_CPPS} ${UTILS_SOURCE_CPPS} simulator.cpp)