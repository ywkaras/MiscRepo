#include <cstring>

#include <ts/string_view.h>

namespace ts {

// String represented internally as a C-style nul-terminated string.
//
class CStringView
{
public:
  CStringView(const char *d = 0) : _data(d) { }

  const char * data() const { return _data; }

  std::size_t length() const { return _data ? std::strlen(_data) : 0; }

  string_view sv() const { return string_view{data(), length()}; }

  operator string_view () const { return sv(); }

private:
  const char *_data;
};

}
