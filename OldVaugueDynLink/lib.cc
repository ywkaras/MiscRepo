#include "foo.h"

extern void call2(void (*)());

extern "C" void lib();

void lib()
{
  // Force creation of vague linkage segment.
  call2(&foo);
}
