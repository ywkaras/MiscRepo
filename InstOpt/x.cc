// See if instances of B will be optimized out.

struct C
{
  int dummy1;
  bool x();
  void y();
};

struct A
{
  virtual void f();
};

struct B : public A
{
  C &c;
  int dummy2;

  B(C &c_) : c(c_), dummy2(0) {}

  void f() override
  {
    while (c.x())
      c.y();

    ++dummy2;
  }
};

void foo(C &c)
{
  B(c).f();
}
