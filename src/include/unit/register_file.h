#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include "common/config.h"

namespace conless {

struct Register {
  int data_{0};
  int dependency_{-1};
};

class RegisterFile {
 private:
  Register regs_[REG_FILE_SIZE];
};

} // namespace conless
#endif