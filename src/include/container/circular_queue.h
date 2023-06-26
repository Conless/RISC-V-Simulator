#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <exception>
#include <iterator>

#include "common/config.h"
#include "common/types.h"

namespace conless {

template <class T>
class circular_queue { // NOLINT
  using value_type = T;

 public:
  auto push(const T &data) -> bool { // NOLINT
    if (size() == MAX_QUEUE_SIZE) {
      return false;
    }
    queue_[(++tail_) % MAX_QUEUE_SIZE] = data;
    return true;
  }
  auto pop() -> const T & { // NOLINT
    if (empty()) {
      throw std::exception();
    }
    return queue_[(head_++) % MAX_QUEUE_SIZE];
  }
  auto front() -> const T & { // NOLINT
    if (empty()) {
      throw std::exception();
    }
    return queue_[head_ % MAX_QUEUE_SIZE];
  }
  auto back() -> const T & { // NOLINT
    if (empty()) {
      throw std::exception();
    }
    return queue_[tail_ % MAX_QUEUE_SIZE];
  }
  auto operator[](const size_t &pos) -> T & { return queue_[pos % MAX_QUEUE_SIZE]; }

 public:
  auto head() -> int { return head_; } // NOLINT
  auto tail() -> int { return tail_; } // NOLINT
  auto size() -> size_t { return tail_ - head_ + 1; } // NOLINT
  auto empty() -> bool { return head_ > tail_; } // NOLINT

 public:
  template <bool const_tag>
  class base_iterator { // NOLINT
    friend class circular_queue;
    friend class base_iterator<true>;
    friend class base_iterator<false>;

   private:
    const circular_queue<T> *iter_{nullptr};
    int pos_{-1};

   public:
    using difference_type = std::ptrdiff_t;
    using value_type = typename circular_queue<T>::value_type;
    using iterator_category = std::output_iterator_tag;
    using pointer = typename std::conditional<const_tag, const value_type *, value_type *>::type;
    using reference = typename std::conditional<const_tag, const value_type &, value_type &>::type;

    base_iterator() = default;
    base_iterator(const circular_queue<T> *iter, int pos) : iter_(iter), pos_(pos) {}
    template <bool _const_tag>
    explicit base_iterator(const base_iterator<_const_tag> &other) : iter_(other.iter_), pos_(other.pos_) {}

    auto operator++(int) -> base_iterator {
      if (pos_ == iter_->tail_) {
        throw std::exception();
      }
      base_iterator cp = *this;
      pos_ = (pos_ + 1) % MAX_QUEUE_SIZE;
      return cp;
    }
    auto operator++() -> base_iterator & {
      if (pos_ == iter_->tail_) {
        throw std::exception();
      }
      pos_ = (pos_ - 1) % MAX_QUEUE_SIZE;
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

    auto operator*() const -> reference { return iter_->queue_[pos_ % MAX_QUEUE_SIZE]; }
    auto operator->() const -> pointer { return &this->queue_[pos_ % MAX_QUEUE_SIZE]; }
  };

  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;

 public:
  auto begin() -> iterator { // NOLINT
    if (empty()) {
      throw std::exception();
    }
    return {this, head_};
  }
  auto end() -> iterator { // NOLINT
    if (empty()) {
      throw std::exception();
    }
    return {this, tail_ + 1};
  }
  

 private:
  int head_{0};
  int tail_{-1};
  T queue_[MAX_QUEUE_SIZE];
};

}  // namespace conless

#endif