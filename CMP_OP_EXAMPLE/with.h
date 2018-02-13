#pragma once

#include <cstring>
#include <string>
#include <string_view>

#include "cmp_op.h"

struct FullName
{
  std::string personal, family;
};

struct FullNameView
{
  std::string_view personal, family;
};

int fullNameCompare(const FullNameView &fnv, const FullName &fn)
{
  auto cmp = [](const std::string_view &sv, const std::string &s) -> int
  {
    std::size_t min_len = sv.size() < s.size() ? sv.size() : s.size();
    int c = memcmp(sv.data(), s.data(), min_len);
    if (c == 0) {
      c = int(sv.size()) - int(s.size());
    }
    return c;
  };
  int c = cmp(fnv.family, fn.family);
  if (c == 0) {
    c = cmp(fnv.personal, fn.personal);
  }
  return c;
}

namespace ts
{
namespace cmp_op
{
template <>
struct Enable<FullNameView, FullName> : public Yes<FullNameView, FullName, fullNameCompare> {};
}
}
