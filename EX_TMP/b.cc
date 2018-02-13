template <typename T>
struct C
{
  static void f();
};

template <> void C<int>::f() {}

template<typename T> void f(T) {}

void g();

int main()
{
  g();
  f(double(777));
  return 0;
};
