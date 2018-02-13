#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>

#include <pthread.h>

using namespace std::chrono_literals;

std::atomic<bool> reader1_lock, reader2_lock;

pthread_rwlock_t rwlock =
#if 1
PTHREAD_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP;
#else
PTHREAD_RWLOCK_INITIALIZER;
#endif

void reader1()
{
  pthread_rwlock_rdlock(&rwlock);

  reader1_lock = true;

  std::this_thread::sleep_for(2s);

  pthread_rwlock_unlock(&rwlock);
}

void writer()
{
  while (!reader1_lock)
    std::this_thread::yield();

  pthread_rwlock_wrlock(&rwlock);

  std::cout << (reader2_lock ? "bad\n" : "good\n");

  pthread_rwlock_unlock(&rwlock);
}

void reader2()
{
  std::this_thread::sleep_for(1s);

  pthread_rwlock_rdlock(&rwlock);

  reader2_lock = true;

  pthread_rwlock_unlock(&rwlock);
}

int main()
{
  std::thread rt(reader1);
  std::thread wt(writer);

  reader2();

  wt.join();
  rt.join();

  return 0;
}
