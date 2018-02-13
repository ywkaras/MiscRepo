#include <iostream>
#include <cstdlib>
#include <atomic>

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

std::atomic<int> flag{0}, run{1};

void *tfunc(void *)
{
  std::atomic<unsigned> i = 0;

  while (!flag) {
    ;
  }
  while (run) {
    ++i;
  }

  if (i) {
    std::cout << i << std::endl;
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
  run = 0;
  flag = 0;

  return nullptr;
}

const int Num_low_priority_threads = 10;

pthread_t thread[Num_low_priority_threads], mon_thread;

int main()
{
  pthread_attr_t pt_attr;

  CHECK(pthread_attr_init(&pt_attr) == 0);
  CHECK(pthread_attr_setscope(&pt_attr, PTHREAD_SCOPE_SYSTEM) == 0);

  for (int j = 0; j < Num_low_priority_threads; ++j) {
    CHECK(pthread_create(thread + j, &pt_attr, tfunc, nullptr) == 0);
  }

  #if 1
  #define SCHED_ SCHED_FIFO
  #else
  #define SCHED_ SCHED_RR
  #endif

  sched_param sparam;
  sparam.sched_priority = sched_get_priority_max(SCHED_);

  #if 0

  CHECK(pthread_attr_setschedpolicy(&pt_attr, SCHED_) == 0);
  CHECK(pthread_attr_setschedparam(&pt_attr, &sparam) == 0);

  CHECK(pthread_create(&mon_thread, &pt_attr, t_mon_func, nullptr) == 0);

  #else

  CHECK(pthread_create(&mon_thread, &pt_attr, t_mon_func, nullptr) == 0);

  CHECK(pthread_setschedparam(mon_thread, SCHED_,  &sparam) == 0);

  #endif

  CHECK(pthread_attr_destroy(&pt_attr) == 0);

  int policy;
  CHECK(pthread_getschedparam(mon_thread, &policy, &sparam) == 0);
  std::cerr << "policy=" << policy <<std::endl;
  CHECK(policy == SCHED_);
  CHECK(sparam.sched_priority == sched_get_priority_max(SCHED_));

  sleep(1);

  #if 0

  flag = 1;

  #else

  int fd = open("/usr/include/stdio.h", O_RDONLY);
  CHECK(fd >= 0);
  CHECK(read(fd, &flag, sizeof(flag)) == ssize_t(sizeof(flag)));

  #endif

  while (run) {
    ;
  }

  void *dummy;

  for (int j = 0; j < Num_low_priority_threads; ++j) {
    CHECK(pthread_join(thread[j], &dummy) == 0);
  }
  CHECK(pthread_join(mon_thread, &dummy) == 0);

  return 0;
}
