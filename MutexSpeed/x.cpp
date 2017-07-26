#include <mutex>

volatile int i;

std::mutex m;

struct X
{
  X()
  {
#ifdef MUTEX
    std::lock_guard<std::mutex> lg(m);
#endif

    ++i;
  }
};

int main()
{
  new X[DIMENSION];

  return(0);
}
