#pragma once

#include <algorithm>
#include <mutex>
#include <vector>

template <class T> class RingBuffer {
public:
  /**
   * Simple Circular buffer implementation, based on the array
   * @param max_size buffer max size
   */
  RingBuffer(const size_t capacity) : CAPACITY(capacity), buffer_(capacity) {}
  /**
  * Puts a chunk of data to the buffer
  * @param data buffer with the data
  * @param size size of the buffer
  * @return true on success
  */
  bool push(const T *data, const size_t size) {
    if (!data || !size) {
      return false;
    }
    guard lock(mtx_);
    if (size_ + size > CAPACITY) {
      return false;
    }
    for (size_t i(0); i < size; i++) {
      buffer_[end_++] = data[i];
      if (end_ >= CAPACITY) {
        end_ = 0;
      }
    }
    size_ += size;
    return true;
  }
  /**
  * Gets a chunk of data from the buffer
  * @param data buffer
  * @param size size of the buffer
  * @return number of elements actually read
  */
  size_t pull(T *data, const size_t size) {
    if (!data || !size) {
      return 0;
    }
    guard lock(mtx_);
    if (!size_) {
      return 0;
    }
    const size_t returnCount = std::min(size_, size);
    for (size_t i(0); i < returnCount; i++) {
      data[i] = buffer_[start_++];
      if (start_ >= CAPACITY) {
        start_ = 0;
      }
    }
    size_ -= returnCount;
    return returnCount;
  }
  bool hasEnoughSpace(const size_t count) const {
    guard lock(mtx_);
    return (size_ + count <= CAPACITY);
  }
  size_t size() const {
    guard lock(mtx_);
    return size_;
  }
  void reset() {
    guard lock(mtx_);
    size_ = 0;
    start_ = 0;
    end_ = 0;
  }
  size_t capacity() const { return CAPACITY; }

private:
  const size_t CAPACITY;
  size_t size_ = 0;
  size_t start_ = 0;
  size_t end_ = 0;
  std::vector<T> buffer_;
  typedef std::lock_guard<std::mutex> guard;
  mutable std::mutex mtx_;
};
