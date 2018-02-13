#include <thread>
#include <fstream>
#include <cstdlib>

#include <unistd.h>
#include <fcntl.h>

void check(bool expr)
{
  if (!expr) {
    std::exit(1);
  }
}

volatile bool f{false};

void tfunc()
{
  while (true) {
    if (f) {
      static_cast<void>(*static_cast<volatile int *>(nullptr));
    }
  }
}

std::thread t[50];

int main()
{
  for (int j = 0; j < 50; ++j) {
    t[j] = std::thread(tfunc);
  }

  std::fstream fs;

  do {
    fs.clear();
    fs.open("/tmp/xx", std::ios_base::in);
  } while (!fs.good());

  fs.close();

  #if 1

  f = true;

  #else

  int fd = open("/usr/include/stdio.h", O_RDONLY);
  check(fd >= 0);
  check(read(fd, const_cast<bool *>(&f), sizeof(f)) == ssize_t(sizeof(f)));

  #endif

  while (true) {
    ;
  }

  return 0;
}
