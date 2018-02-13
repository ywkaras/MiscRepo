#include "InterprocessQueue.h"

#include <pthread.h>

#define SKEWED_YIELDS 0

struct TestElem : public InterprocessQueueElem
{
  int i;
};

InterprocessQueue<TestElem> q;

const int NumElem = 10000;

TestElem e[NumElem + 3];

#include <thread>
#include <iostream>
#include <cstdlib>

pthread_t
thread_create(void *(*f)(void *))
{
  // Thread start code copied from Traffic Server.

  pthread_t thread_id;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  int ret = pthread_create(&thread_id, &attr, f, nullptr);
  if (ret != 0) {
    std::cout << "pthread_create() failed\n";
    std::exit(1);
  }
  pthread_attr_destroy(&attr);

  return thread_id;
}

void thread_join(pthread_t thread_id)
{
  void *dummy;
  if (pthread_join(thread_id, &dummy)) {
    std::cout << "pthread_join() failed\n";
    std::exit(1);
  }
}
    

void * produce(void *)
{
  for (int i = 0; i < (NumElem + 3); ++i) {
    q.enqueue(e[i]);
#if SKEWED_YIELDS
    if (!(i % 7)) {
      std::this_thread::yield();
    }
#endif
  }

  return nullptr;
}

void consume(int id)
{
#if SKEWED_YIELDS
  int i = 0;
#endif
  for ( ; ; ) {
    TestElem &e_ = q.dequeue();
    if ((&e_ - e) >= NumElem) {
      break;
    }
    if (e_.i != 0) {
      std::cout << "Error: consumer=" << id << " elem=" << &e_ - e << '\n';
    }
    e_.i = id;
#if SKEWED_YIELDS
    if (++i == (11 * (id - 1))) {
      std::this_thread::yield();
      i = 0;
    }
#endif
  }
}

void * cons1(void *) { consume(1); return nullptr; }
void * cons2(void *) { consume(2); return nullptr; }

#include <thread>

int main()
{
  // std::thread cons1(consume, 1);
  // std::thread cons2(consume, 2);
  // std::thread prod(produce);

  pthread_t c1 = thread_create(cons1);
  pthread_t c2 = thread_create(cons2);
  pthread_t p = thread_create(produce);

  consume(3);

  // cons2.join();
  // cons1.join();
  // prod.join();

  thread_join(c2);
  thread_join(c1);
  thread_join(p);

  int i, sum, all = NumElem;

  for (int id = 1; id <= 3 ; ++id) {
    for (sum = 0, i = 0; i < NumElem; ++i) {
      if (e[i].i == id) {
        ++sum;
      }
    }
    all -= sum;
    std::cout << "consumer " << id << ": " << sum << '\n';
  }

  if (all != 0) {
    std::cout << "bad elems = " << all << '\n';
  }

  return 0;
}
