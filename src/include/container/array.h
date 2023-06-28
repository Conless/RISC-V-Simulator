#ifndef ARRAY_H
#define ARRAY_H

#include <cstddef>
#include <cstring>
#include <iterator>

namespace conless {

template <class T, int SIZE>
class array {  // NOLINT
 public:
  using value_type = std::pair<bool, T>;

  array() {
    for (int i = 0; i < SIZE; i++) {
      data_[i].first = false;
    }
  }

  auto space() -> int {  // NOLINT
    for (int i = 0; i < SIZE; i++) {
      if (!data_[i].first) {
        return i;
      }
    }
    return -1;
  }
  auto full() -> bool { return space() == -1; }   // NOLINT
  inline auto capacity() -> int { return SIZE; }  // NOLINT
  auto erase(const int pos) -> bool {             // NOLINT
    if (!data_[pos].first) {
      return false;
    }
    data_[pos].first = false;
    return true;
  }
  auto erase_move(const int pos) -> bool {  // NOLINT
    if (!data_[pos].first) {
      return false;
    }
    for (int i = pos; i < SIZE - 1; i++) {
      data_[i] = data_[i + 1];
      if (!data_[i].first) {
        return true;
      }
    }
    data_[SIZE - 1].first = false;
    return true;
  }
  auto operator[](const int pos) -> T & {
    data_[pos].first = true;
    return data_[pos].second;
  }
  auto busy(const int pos) -> bool & { return data_[pos].first; }  // NOLINT
  void clean() { // NOLINT
    for (int i = 0; i < SIZE; i++) {
      data_[i].first = false;
    }
  }

 public:
  template <bool const_tag>
  class base_iterator {  // NOLINT
    friend class array;
    friend class base_iterator<true>;
    friend class base_iterator<false>;

   private:
    array<T, SIZE> *iter_{nullptr};
    int pos_{-1};

   public:
    using difference_type = std::ptrdiff_t;
    using value_type = typename array<T, SIZE>::value_type;
    using iterator_category = std::output_iterator_tag;
    using pointer = typename std::conditional<const_tag, const value_type *, value_type *>::type;
    using reference = typename std::conditional<const_tag, const value_type &, value_type &>::type;

    base_iterator() = default;
    base_iterator(array<T, SIZE> *iter, int pos) : iter_(iter), pos_(pos) {}
    template <bool _const_tag>
    explicit base_iterator(const base_iterator<_const_tag> &other) : iter_(other.iter_), pos_(other.pos_) {}

    auto operator++(int) -> base_iterator {
      auto cp = *this;
      for (int i = pos_ + 1; i < SIZE; i++) {
        if (iter_->data_[i].first) {
          pos_ = i;
          return cp;
        }
      }
      pos_ = SIZE;
      return cp;
    }
    auto operator++() -> base_iterator & {
      for (int i = pos_ + 1; i < SIZE; i++) {
        if (iter_->data_[i].first) {
          pos_ = i;
          return *this;
        }
      }
      pos_ = SIZE;
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

    auto exist() -> bool { return iter_->data_[pos_].first; }  // NOLINT
    auto operator*() const -> reference {
      if (!iter_->data_[pos_].first) {
        throw std::exception();
      }
      return iter_->data_[pos_];
    }
    auto operator->() const -> pointer {
      if (!iter_->data_[pos_].first) {
        throw std::exception();
      }
      return &this->data_[pos_];
    }
  };

  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;

  auto begin() -> iterator {  // NOLINT
    for (int i = 0; i < SIZE; i++) {
      if (data_[i].first) {
        return {this, i};
      }
    }
    return {this, SIZE};
  }
  auto end() -> iterator { return {this, SIZE}; }  // NOLINT

 private:
  std::pair<bool, T> data_[SIZE];
};


}  // namespace conless
#endif