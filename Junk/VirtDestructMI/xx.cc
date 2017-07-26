#include <iostream>

using std::cout;

struct A
{
  virtual ~A() { cout << "A\n"; }
};

struct B
{
  ~B() { cout << "B\n"; }
};

struct C : public A, public B
{
  ~C() { cout << "C\n"; }
};

C c;

int main() { return 0; }
