#include "Dynamic_obj.h"

struct X
{
  int i;
  bool flag;

  X(int i_, bool flag_) : i(i_), flag(flag_) {}
};

void foo()
{
  X *xp = dynamic_obj::create<X>(5, true);

  dynamic_obj::destroy(xp);

  dynamic_obj::create(xp, 10, false);

  dynamic_obj::destroy(xp);
}
