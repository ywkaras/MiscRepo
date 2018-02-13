#include <pthread.h>
#include <iostream>
#include <sstream>
 
constexpr int NumberThreads = 100;
 
//----------------------------------------------------------------------------
class Thing {
  public:
  Thing() {
    id = pthread_self();
  }
  pthread_t id;
};
 
//----------------------------------------------------------------------------
class Test {
  public:
  static void use_it() {
    auto id = pthread_self();
    if (id != thing.id) {
      std::ostringstream os;
      os << "failed for id=" << id << '\n';
      std::cerr << os.str();
    }
  }
  static thread_local Thing thing;
};
 
thread_local Thing Test::thing;
 
//----------------------------------------------------------------------------
void *child(void *)
{
  Test::use_it();
 
  return nullptr;
}
 
//----------------------------------------------------------------------------
int main() {
  pthread_t thread_pool[NumberThreads];
 
  for (intptr_t i = 0; i < NumberThreads; ++i) {
    pthread_create(&thread_pool[i], nullptr, child, (void*)i);
  }
 
  for (int i = 0; i < NumberThreads; ++i) {
    pthread_join(thread_pool[i], nullptr);
  }
 
  return(0);
}
