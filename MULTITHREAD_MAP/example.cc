#include "mt_map.h"

// This is just to check that the code compiles.

using M = Map<int, double>;
M m;

void foo(M::Node const *&np)
{
  np = m.find(666);

  M::Node n;
  n.key = 666;
  n.value = 3.14;

  m.insert(n);
}
