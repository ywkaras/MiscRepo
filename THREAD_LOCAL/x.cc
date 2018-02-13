int x();

thread_local int i{x()};

void f() { i = 0xff; }
int g() { return i; }
