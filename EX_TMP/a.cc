template <typename T>
struct C
{
  static void f();
};

template <typename T>
void f(T);

void g()
{
  C<int>::f();
  f(double(666));
}
