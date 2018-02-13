#pragma once

#include <iostream>

inline void foo()
{
#ifdef LIB
  std::cout << "lib\n";
#else
  std::cout << "core\n";
#endif
}
