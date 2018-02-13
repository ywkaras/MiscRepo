#include "string_view.h"

class BufferWriter;

void operator << (BufferWriter &bw, ts::string_view sv);
void operator << (BufferWriter &bw, char c);

class PrintVA
{
public:
  struct W
  {
    const int w;
    W(int w_) : w(w_) { };
  };

  template <typename ... Rest>
  PrintVA(BufferWriter& bw, Rest const& ... rest)
  {
    _x(bw, rest ...);
  }

  operator int () { return _fieldCount; }

private:
  int _fieldCount = 0;
  int _width = 0;

  template <typename ... Rest>
  void _x(BufferWriter& bw, ts::string_view sv, Rest const& ... rest)
  {
    while (unsigned(_width) > sv.length()) {
      bw << ' ';
      --_width;
    }

    _width = 0;

    bw << sv;

    ++_fieldCount;

    _x(bw, rest ...);
  }

  template <typename ... Rest>
  void _x(BufferWriter& bw, const char *cStr, Rest const& ... rest)
  {
    _x(bw, ts::string_view(cStr), rest ...);
  }

  template <typename ... Rest>
  void _x(BufferWriter& bw, W width, Rest const& ... rest)
  {
    _width = width.w;

    _x(bw, rest ...);
  }

  template <typename ... Rest>
  void _x(BufferWriter& bw, int i, Rest const& ... rest); // ...

  void _x() {}
};

int foo(BufferWriter &bw)
  {
    using W = PrintVA::W;
    return PrintVA(bw, "One ", 1, " abc ", "twooo", " def ", W(150), "threee", " ghi ",  W(2), "5");
  }
