#include "WrapDbg.h"

volatile int j;

Wrap<int> i;

int main()
{
  i = 10;

  j = i;

  return(0);
}
