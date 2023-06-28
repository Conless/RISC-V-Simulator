#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <exception>
#include <iterator>
#include "common/config.h"

namespace conless {

template <class T, int SIZE = MAX_QUEUE_SIZE>
class circular_queue {  // NOLINT
  using value_type = T;

 public:
  auto push(const T &data) -> bool {  // NOLINT
    if (size() == SIZE) {
      return false;
    }
    queue_[(++tail_) % SIZE] = data;
    return true;
  }
  auto pop() -> const T & {  // NOLINT
    if (empty()) {
      throw std::exception();
    }
    return queue_[(head_++) % SIZE];
  }
  void clean() { head_ = tail_ + 1; } // NOLINT
  auto front() -> T & {  // NOLINT
    if (empty()) {
      throw std::exception();
    }
    return queue_[head_ % SIZE];
  }
  auto back() -> T & {  // NOLINT
    if (empty()) {
      throw std::exception();
    }
    return queue_[tail_ % SIZE];
  }
  auto operator[](const size_t &pos) -> T & { return queue_[pos % SIZE]; }

 public:
  auto head() -> int { return head_; }                      // NOLINT
  auto tail() -> int { return tail_; }                      // NOLINT
  auto size() -> size_t { return tail_ - head_ + 1; }       // NOLINT
  auto empty() -> bool { return head_ > tail_; }            // NOLINT
  auto full() -> bool { return size() == SIZE; }  // NOLINT

 public:
  template <bool const_tag>
  class base_iterator {  // NOLINT
    friend class circular_queue;
    friend class base_iterator<true>;
    friend class base_iterator<false>;

   private:
    circular_queue<T, SIZE> *iter_{nullptr};
    int pos_{-1};

   public:
    using difference_type = std::ptrdiff_t;
    using value_type = typename circular_queue<T, SIZE>::value_type;
    using iterator_category = std::output_iterator_tag;
    using pointer = typename std::conditional<const_tag, const value_type *, value_type *>::type;
    using reference = typename std::conditional<const_tag, const value_type &, value_type &>::type;

    base_iterator() = default;
    base_iterator(circular_queue<T, SIZE> *iter, int pos) : iter_(iter), pos_(pos) {}
    template <bool _const_tag>
    explicit base_iterator(const base_iterator<_const_tag> &other) : iter_(other.iter_), pos_(other.pos_) {}

    auto operator++(int) -> base_iterator {
      if (*this == iter_->end()) {
        throw std::exception();
      }
      base_iterator cp = *this;
      pos_ = (pos_ + 1) % SIZE;
      return cp;
    }
    auto operator++() -> base_iterator & {
      if (*this == iter_->end()) {
        throw std::exception();
      }
      pos_ = (pos_ + 1) % SIZE;
      return *this;
    }

    template <bool _const_tag>
    auto operator==(const base_iterator<_const_tag> &rhs) const -> bool {
      return iter_ == rhs.iter_ && pos_ == rhs.pos_;
    }
    template <bool _const_tag>
    auto operator!=(const base_iterator<_const_tag> &rhs) const -> bool {
      return iter_ != rhs.iter_ || pos_ != rhs.pos_;
    }

    auto operator*() const -> reference { return iter_->queue_[pos_ % SIZE]; }
    auto operator->() const -> pointer { return &this->queue_[pos_ % SIZE]; }
  };

  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;

 public:
  auto begin() -> iterator {  // NOLINT
    return {this, head_};
  }
  auto end() -> iterator {  // NOLINT
    return {this, tail_ + 1};
  }

 private:
  int head_{0};
  int tail_{-1};
  T queue_[SIZE];
};

}  // namespace conless

#endif