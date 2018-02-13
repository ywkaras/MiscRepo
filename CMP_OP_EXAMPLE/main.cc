#if 0
#include "with.h"
#else
#include "without.h"
#endif

#include <iostream>
#include <cstdlib>

void test1(bool f)
{
  if (!f) {
    std::cout << "FAIL\n";
    std::exit(1);
  }
}

template <typename T1, typename T2>
void test(T1 &&less, T2 &&greater)
{
  test1(less < greater);
  test1(less <= greater);
  test1(less != greater);
  test1(!(less == greater));
  test1(!(less > greater));
  test1(!(less >= greater));

  test1(greater > less);
  test1(greater >= less);
  test1(greater != less);
  test1(!(greater == less));
  test1(!(greater < less));
  test1(!(greater <= less));
}

int main()
{
  FullNameView fnv;
  FullName fn;

  fnv.personal = "Bob";
  fnv.family = "Jones";
  fn.personal = "Bob";
  fn.family = "Smith";

  test(fnv, fn);

  fnv.personal = "Bob";
  fnv.family = "Smith";
  fn.personal = "John";
  fn.family = "Smith";

  test(fnv, fn);

  fn.personal = "Bob";
  fn.family = "Jones";
  fnv.personal = "Bob";
  fnv.family = "Smith";

  test(fn, fnv);

  fn.personal = "Bob";
  fn.family = "Smith";
  fnv.personal = "John";
  fnv.family = "Smith";

  test(fn, fnv);

  std::cout << "SUCCESS\n";
  return 0;
}
