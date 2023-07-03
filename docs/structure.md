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
  void Execute();
  void Commit();

 private:
  Memory memory_;
  InstructionUnit ins_unit_;
  ReorderBuffer ro_buffer_;
  RegisterFile reg_file_;
  LoadStoreUnit ls_unit_;
  ReservationStation res_station_;
  ArithmeticLogicUnit alu_;

 private:
  Bus cd_bus_, mem_bus_;
  Wire wire_[20];
  long pc_;
};
```

The function ```InitMemory()``` is designed to read all the input data into RAM.

The function ```Run()``` is designed to execute the program from the given program counter, it will use the functions below:
1. Fetch an instruction and decode it: Using ```FetchDecode()``` to awake iu, order it to fetch and decode an instruction and push it into instruction queue.
2. Issue an instruction: order iu to issue an instruction to rob and rss / lsb.
3. Execute an operation in rob.
4. Commit an operation. If it is a load / arith operation, write it to register file. If it is a store operation, push it to lsb.

### Each units
```cpp
class InstructionUnit {
 public:
  void FetchDecode(State &current_state);
  void Issue(State &current_state);
 private:
  queue<InsType> ins_queue_;
};
```
1. The function ```FetchDecode()``` get a instruction by the given pc, decode it and push it into the ins_queue_, it will update the state of pc by predictor;
2. The function ```Issue()``` get a instruction from the head of ins_queue_, and push it into the ro_buffer_ and res_station_ by checking the state of reg_file_.

```cpp
struct RssEntry {
  InsType ins_;
  int rob_pos_;
  int v1_, v2_;
  int q1_, q2_;
  int num_;
  int dest_;
};
class ReservationStation {
 public:
  void Execute(State &current_state);
 private:
  circular_queue<RssEntry> ls_entries_, arith_entries_;
};
```
1. For arith operation, the function ```Execute()``` check the entries in arith, execute its result and push the {rob_pos_, result} pair into bus.
2. For load operation, it push the {age, address, rob_pos_} pair into LoadStoreUnit by wire.
3. For store operation, it push the {age, rob_pos_, store_num} pair into LoadStoreUnit and push the
   
```cpp
struct RobEntry {
  InsType ins_;
  InsState state_;
  int dest_;
  int dest_offset_;
  int value_;
};
class ReorderBuffer {
 public:
  void MonitorCdb(State &current_state);
  void Commit(State &current_state);
 private:
  circular_queue<ReorderBuffer> entries_;
  Bus *cdb_;
}
```
1. For branch operation, the function ```Commit()``` checks if the predict is correct.
2. For arith and load operation, it store the data into register file.
3. For store operation, it access the LoadStoreUnit by wire

### class Memory and Bus
```cpp
constexpr int MAX_RAM_SIZE = 1 << 20;
class Memory {
 public:
  void Init();
  void MonitorBus(Bus );
 private:
  uint8_t ram_[MAX_RAM_SIZE];
  Bus *memb_;
};

class Bus {
 public:
  void Broadcast();
};

class Wire {
 public:
  int data;
};
```