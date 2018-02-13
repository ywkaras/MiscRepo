#include <iostream>

#include "A.h"

#if !INLINE_FUNC_SINGLETON

A A::onlyInstance;

#endif

int main()
{
  #if INLINE_FUNC_SINGLETON

  std::cout << "inline function singleton\n";

  #else

  std::cout << "static memeber singleton\n";

  #endif

  if (&A::getSingleton() == inLib()) {

    std::cout << "match\n";

  } else {

    std::cout << "match failed\n";

  }

  return 0;
}
