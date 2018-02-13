// Test member function pointer casting from derived to base.

struct A {};

struct B : public A
{
  #if 1
  virtual
  #endif
  void f();
};

void (A::*fp)() = static_cast<void (A::*)()>(&B::f);

#include <iostream>

void B::f() { std::cout << "f\n"; }

int main()
{
  B b;
  A *ap = &b;
  (ap->*fp)();

  return 0;
}
