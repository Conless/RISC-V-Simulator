#ifndef CONFIG_H
#define CONFIG_H

// #define SHOW_ALL

namespace conless {

constexpr int MAX_QUEUE_SIZE = 32;

constexpr int MAX_ROB_SIZE = 32;

constexpr int MAX_RAM_SIZE = 1 << 20;

constexpr int REG_FILE_SIZE = 32;

constexpr int MAX_RSS_SIZE = 10;

constexpr int BUS_WIDTH = 6;
}  // namespace conless

#endif