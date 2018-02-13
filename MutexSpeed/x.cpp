#include <mutex>
#include <thread>
#include <iostream>
#include <time.h>

const unsigned long long Seconds = 5;
const unsigned long long Nanoseconds = Seconds * 1000 * 1000 * 1000;

volatile int dummy;

template <typename Guard>
unsigned long long loop_count()
{
  unsigned long long count = 0;
  timespec ts, ts_end;

  clock_gettime(CLOCK_REALTIME, &ts_end);

  ts_end.tv_sec += Seconds;

  do {
    {
      Guard g;
      dummy = 1;
    }

    clock_gettime(CLOCK_REALTIME, &ts);

    count++;

  } while (not (((ts.tv_sec == ts_end.tv_sec) and (ts.tv_nsec >= ts_end.tv_nsec)) or (ts.tv_sec > ts_end.tv_sec)));

  return count;
}

std::mutex m;

struct Mutex_guard : public std::lock_guard<std::mutex>
{
  Mutex_guard() : std::lock_guard<std::mutex>(m) {};
};

struct Null_guard {};

int main()
{
  double mtx = double(Nanoseconds) / loop_count<Mutex_guard>();
  double base = double(Nanoseconds) / loop_count<Null_guard>();

  std::cout << "Mutex lock + free nanoseconds: " << (mtx - base) << '\n';

  return(0);
}
