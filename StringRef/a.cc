#include <string>
#include <iostream>

static const std::string & s("xxx");

void p1() { std::cout << std::hex << reinterpret_cast<unsigned long>(&s) << std::dec << std::endl; }
