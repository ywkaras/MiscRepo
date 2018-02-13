#include <iostream>
#include <cxxabi.h>

int main(int n_arg, const char * const arg[])
{
  int status;
  const char *realname;

  for (int i = 1; i < n_arg; ++i)
  {
    char *res = abi::__cxa_demangle(arg[i], 0, 0, &status);
    if (res)
      realname = res;
    else
      realname = "NOT VALID";
    std::cout << arg[i] << " => " << realname << " : " << status << '\n';
    free(res);
  }

  return 0;
}
