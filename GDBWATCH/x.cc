#include <thread>
#include <fstream>

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

  f = true;

  while (true) {
    ;
  }

  return 0;
}
