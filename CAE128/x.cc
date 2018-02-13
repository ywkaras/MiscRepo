/*
Program to test the overhead of multiple cores doing coherent updating of the same __int128_t with heavy
contention and strong memory ordering, versus multiple cores doing read-increment-write of the same __int128_t
with no memory ordering, versus doing read-increment-write of different __int128_t with no memory ordering.
*/

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <cassert>

#define BUS_LOCK 0
#define INCR_SAME 1
#define INCR_DIFF_CACHE_LINE 2
#define NO_INCR 3

#define TEST BUS_LOCK

int const Num_threads = 8;

using CT = __int128_t;

std::ostream &pct(CT v)
{
  CT Max{1000};
  int pow{0};
  if (v < 0) {
    v = -v / 10;
    v = ((CT(1) << 127) / 5) - v;
    pow = 1;
  }
  while (v > Max) {
    v /= 10;
    ++pow;
  }
  std::cout << unsigned(v);
  if (pow) {
    std::cout << 'e' << pow;
  }
  return std::cout;
}

#if TEST == BUS_LOCK

CT global;

#elif TEST == INCR_SAME

CT volatile global;

#elif TEST == INCR_DIFF_CACHE_LINE

// Increment counters in different cache lines.
//
CT volatile dummy[Num_threads][4];

#endif

struct { CT count; CT dummy[3]; } per_thread[Num_threads];

std::atomic<bool> start, done;

void thread_func(int idx)
{
  while (!start) {
    ;
  }

  while (!done) {
    ++per_thread[idx].count;

    #if TEST == BUS_LOCK

    CT curr = __sync_val_compare_and_swap(&global, 0, 0);
    CT expected;
    do {
      expected = curr;
      __sync_synchronize();
      curr = __sync_val_compare_and_swap(&global, expected, expected + 1);
      __sync_synchronize();
    } while (curr != expected);

    #elif TEST == INCR_SAME

    CT curr;
    curr = global;
    global = curr + 1;

    #elif TEST == INCR_DIFF_CACHE_LINE

    CT curr;
    curr = dummy[idx][0];
    dummy[idx][0] = curr + 1;

    #endif
  }
}

std::thread t[Num_threads];

int main()
{
  using namespace std::chrono_literals;

  for (int i = 0; i < Num_threads; ++i) {
    t[i] = std::thread(thread_func, i);
  }

  start = true;

  std::this_thread::sleep_for(20s);

  done = true;

  for (int i = 0; i < Num_threads; ++i) {
    t[i].join();
  }

  CT total{0};

  for (int i = 0; i < Num_threads; ++i) {
    total += per_thread[i].count;
    std::cout << "per_thread[" << i << "] = ";
    pct(per_thread[i].count) << '\n';
  }

  std::cout << "\ntotal = ";
  pct(total) << '\n';

  #if TEST == BUS_LOCK

  assert(global == total);

  #endif

  return 0;
}
