#ifndef BASE_UNIT_H
#define BASE_UNIT_H

namespace conless {

class State;

class BaseUnit {
  virtual void Flush(State *current_state) = 0;
  virtual void Execute(State *current_state, State *next_state) = 0;
};

}

#endif