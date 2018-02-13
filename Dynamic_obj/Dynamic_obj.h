#pragma once

#include <cstddef>
#include <atomic>
#include <utility>
#include <new>

namespace dynamic_obj
{
namespace detail
{
const std::size_t Unit_size = sizeof(void *);

// Sizes (in bytes) greater than this are considered "large".
const std::size_t Large_threshold = 1024;

template <std::size_t Units, bool Is_large>
class Raw;

template <std::size_t Units>
class Raw<Units, false>
{
public:

  static void *
  alloc()
  {
    Data &d = data();
    void *fh = d.free_head;

    for ( ; ; ) {
      if (!fh) {
        return new char [Units * Unit_size];
      }
      // Attempt to pop memory block.
      void *next = *static_cast<void **>(fh);
      if (d.free_head.compare_exchange_weak(fh, next)) {
        --d.free_count;
        return fh;
      }
    }
  }

  static void
  free(void *p)
  {
    if (!p) {
      return;
    }

    Data &d = data();
    void *fh = d.free_head;

    for ( ; ; ) {
      if (d.free_count >= Max_free_count) {
        delete [] static_cast<char *>(p);
        return;
      }
      // Attempt to push memory block.
      *static_cast<void **>(p) = fh;
      if (d.free_head.compare_exchange_weak(fh, p)) {
        ++d.free_count; 
        return;
      }
    }
  }

private:
  static const std::size_t Limit_total_free_size = 32 * 1024;
  static const std::size_t Max_free_count = Limit_total_free_size / (Units * Unit_size);
  
  struct Data
  {
    // Pointer to top of linked-list stack of blocks of memory, each of Units * Unit_size bytes long.
    std::atomic<void *> free_head{nullptr};

    // Count of memory blocks in stack.
    std::atomic<std::size_t> free_count{0};
  };

  static Data & data()
  {
    static Data d;
    return d;
  }
};

template <std::size_t Units>
class Raw<Units, true>
{
public:

  static void *
  alloc()
  {
    return new char [Units * Unit_size];
  }

  static void
  free(void *p)
  {
    delete [] static_cast<char *>(p);
  }
};

} // end namespace detail

template <std::size_t Size>
using Raw = detail::Raw<(Size + detail::Unit_size - 1) / detail::Unit_size, (Size >= detail::Large_threshold)>;

// Explicit specialization needed.
template <typename T, typename... Args>
T *
create(Args&&... args)
{
  void *p = Raw<sizeof(T)>::alloc();
  new(p) T(std::forward<Args>(args)...);
  return static_cast<T *>(p);
}

// No explicit specialization needed.
template <typename T, typename... Args>
void
create(T *&p, Args&&... args)
{
  void *pr = Raw<sizeof(T)>::alloc();
  new(pr) T(std::forward<Args>(args)...);
  p = static_cast<T *>(pr);
}

template <typename T>
void
destroy(T *p)
{
  p->~T();
  Raw<sizeof(T)>::free(p);
}

} // end namespace dynamic_obj
