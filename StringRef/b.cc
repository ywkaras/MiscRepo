#include <string>
#include <iostream>

static const std::string & s("xxx");

extern void p1();

void p2() { std::cout << std::hex << reinterpret_cast<unsigned long>(&s) << std::dec << std::endl; }

int main() { p1(); p2(); }
