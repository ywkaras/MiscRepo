#include <iostream>

#define MACRO std::cout << "a\n";

#include "h.h"

void (* volatile fp2)();

void a()
{
  foo();
  fp2 = foo;
  fp2();
}
