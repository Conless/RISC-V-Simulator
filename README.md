# RISC-V Simulator
## Introduction
RISC-V Simulator is the first lab of PPCA (CS1952@SJTU, 2023 Summer). It simulates the operation of a risc-v cpu, excecute the RV32I instructions and output the result.

The simulator is based on Tomasulo Structure with 1 GB Memory. The base frequency of the simulator is about 7.3 MHz, and IPC for RV32I is about 0.61. The branch predictor of it is dynamic branch prediction with perceptrons.

## How to use it

Run command ```cmake . && make``` to compile it and run by ```./code```.

## Structure
For more details about the structure and operations, please see [Structure Document](docs/structure.md). (Unfinished)
