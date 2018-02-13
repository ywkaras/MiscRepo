#include <iostream>
#include <cstdlib>
#include <atomic>
#include <cstdint>

#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

void check(bool expr, int line)
{
  if (!expr) {
    std::cerr << "FAILED on line " << line << std::endl;
    std::exit(1);
  }
}

#define CHECK(EXPR) check((EXPR), __LINE__)

std::atomic<int> flag{0};

int const Num_low_priority_threads = 10;

unsigned volatile i[Num_low_priority_threads];

void *tfunc(void *idx)
{
  unsigned volatile *pi = i + reinterpret_cast<std::uintptr_t>(idx);

  while (!flag) {
    ;
  }
  while (true) {
    ++*pi;
  }

  return nullptr;
}

// Note:  The intention is that this thread is always running when the process is running, so this program will
// only work on a multi-core CPU.
//
void *t_mon_func(void *)
{
  while (!flag) {
    ;
  }
  std::abort();

  return nullptr;
}

pthread_t thread[Num_low_priority_threads], mon_thread;

int main()
{
  pthread_attr_t pt_attr;

  CHECK(pthread_attr_init(&pt_attr) == 0);
  CHECK(pthread_attr_setscope(&pt_attr, PTHREAD_SCOPE_SYSTEM) == 0);

  for (std::uintptr_t j = 0; j < Num_low_priority_threads; ++j) {
    CHECK(pthread_create(thread + j, &pt_attr, tfunc, reinterpret_cast<void *>(j)) == 0);
  }

  #if 0
  #define SCHED_ SCHED_FIFO
  #else
  #define SCHED_ SCHED_RR
  #endif

  sched_param sparam;
  sparam.sched_priority = sched_get_priority_max(SCHED_);

  CHECK(pthread_create(&mon_thread, &pt_attr, t_mon_func, nullptr) == 0);

  CHECK(pthread_setschedparam(mon_thread, SCHED_,  &sparam) == 0);

  CHECK(pthread_attr_destroy(&pt_attr) == 0);

  int policy;
  CHECK(pthread_getschedparam(mon_thread, &policy, &sparam) == 0);
  std::cerr << "policy=" << policy <<std::endl;
  CHECK(policy == SCHED_);
  CHECK(sparam.sched_priority == sched_get_priority_max(SCHED_));

  sleep(1);

  flag = 1;

  while (true) {
    ;
  }

  return 0;
}
