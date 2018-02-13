#include <utility>

template <typename Callable, typename ...Arg_type>
void g(Callable f, Arg_type&&... arg)
{
  f(std::forward<Arg_type>(arg)...);
}

void ff(int, double, char const *);

void gg()
{
 g(ff, 42, double(6.66), "abc");
}
