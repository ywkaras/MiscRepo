#include <iostream>

#define MACRO std::cout << "main\n";

#include "h.h"

extern void a();

void (* volatile fp1)();

int main()
{
  a();
  foo();
  fp1 = foo;
  fp1();

  return 0;
}
