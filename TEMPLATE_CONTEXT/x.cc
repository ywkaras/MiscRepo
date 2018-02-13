#include <iostream>

template <typename T>
void tpl_func(T x) { foo(x); }

#if PRIMITIVE

using A = int;

#else

struct A {};

#endif

void foo(A) { std::cout << "i x\n"; }

void bar()
{
  tpl_func(A());
}
