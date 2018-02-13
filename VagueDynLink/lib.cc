#include "foo.h"

extern "C" void lib();

void lib()
{
  static_cast<void>(foo());
}
