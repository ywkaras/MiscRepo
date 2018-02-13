#include <fstream>
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

std::atomic<int> f{0};

void *tfunc(void *)
{
  std::atomic<unsigned> i = 0;
  while (true) {
    if (f) {
      ++i;
    }
  }
  return nullptr;
}

static void break_on()
{
  // Just some code that can't get optimized out.
  //
  static volatile int i;
  i = 666;
}

// Note:  The intention is that this thread is always running when the process is running, so this program will
// only work on a multi-core CPU.
//
void *t_mon_func(void *)
{
  while (true) {
    if (f) {
      break_on();
    }
  }
  return nullptr;
}

pthread_t thread[50], mon_thread;

int main()
{
  pthread_attr_t pt_attr;

  CHECK(pthread_attr_init(&pt_attr) == 0);
  CHECK(pthread_attr_setscope(&pt_attr, PTHREAD_SCOPE_SYSTEM) == 0);

  for (int j = 0; j < 50; ++j) {
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

  std::fstream fs;

  do {
    fs.clear();
    fs.open("/tmp/xx", std::ios_base::in);
  } while (!fs.good());

  fs.close();

  #if 0

  f = 1;

  #else

  int fd = open("/usr/include/stdio.h", O_RDONLY);
  CHECK(fd >= 0);
  CHECK(read(fd, &f, sizeof(f)) == ssize_t(sizeof(f)));

  #endif

  while (true) {
    ;
  }

  return 0;
}
