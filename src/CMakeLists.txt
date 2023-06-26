set(PROJECT_NAME ${CMAKE_PROJECT_NAME})

set(UNIT_SOURCE_CPPS
  unit/instruction_unit.cpp
)

set(STORAGE_SOURCE_CPPS
  storage/memory.cpp
)

add_library(simulator ${UNIT_SOURCE_CPPS} ${STORAGE_SOURCE_CPPS} simulator.cpp)