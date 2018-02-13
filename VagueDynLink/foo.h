#pragma once

inline int foo()
{
  static int static_local;
  return ++static_local;
}
