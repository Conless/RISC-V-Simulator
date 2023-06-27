#ifndef LOAD_STORE_BUFFER_H
#define LOAD_STORE_BUFFER_H

#include "container/circular_queue.h"

namespace conless {

enum LsType { LD, ST };

struct LsbEntry {
  LsType type_;
  int begin_addr_, end_addr_;
  int dest_;
  int data_, data_addr_;
};

class LoadStoreBuffer {
 private:
  circular_queue<LsbEntry> load_buffer_, store_buffer_;
};

}  // namespace conless

#endif