# Structure
## Class Level
Simulator
 ├── Memory
 │   ├── Cache
 │   └── RAM
 └── CPU
     ├── Instruction Unit
     │   ├── Fetcher
     │   ├── Decoder
     │   └── Issue Unit
     ├── Reservation Station
     ├── Arithmetic Login Unit
     ├── Load / Store Buffer
     ├── Reorder Buffer
     └── Register File

## Interface
### Top module: class Simulator
```cpp
class Simulator {
 public:
  void InitMemory();
  void Run(long start_pc);

 protected:
  void FetchDecode();
  void Issue();
  void ExecuteArith();
  void ExecuteLoad();
  void Commit();

 private:
  Memory memory_;
  InstructionUnit iu_;
  ReorderBuffer rob_;
  RegisterFile reg_;
  LoadStoreBuffer lb_;
  ReservationStation rss_;
  ArithmeticLogicUnit alu_;

 private:
  Bus cdb_, memb_;
  Wire wire_[20];
  long pc_;
};
```

The function ```InitMemory()``` is designed to read all the input data into RAM.

The function ```Run()``` is designed to execute the program from the given program counter, it will use the functions below:
1. Fetch an instruction and decode it: Using ```FetchDecode()``` to awake iu, order it to fetch and decode an instruction and push it into instruction queue.
2. Issue an instruction: order iu to issue an instruction to rob and rss / lsb.
3. Execute an arithmetic operation in rss and push it back to cdb.
4. Execute an load operation in lsb and push it to both memb and cdb.
5. Commit an operation. If it is a load / arith operation, write it to register file. If it is a store operation, push it to lsb.
