#include "foo.h"

extern "C" void lib();

extern void call(void (*)());

int main()
{
  // Force creation of vague linkage segment.
  call(&foo);

  lib();

  return 0;
}
