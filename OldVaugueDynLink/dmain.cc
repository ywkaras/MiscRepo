#include <cstdlib>

#include <dlfcn.h>

#include "foo.h"

extern "C" void lib();

extern void call(void (*)());

int main()
{
  // Force creation of vague linkage segment.
  call(&foo);

  void *lib_handle = dlopen("./lib.a", RTLD_NOW);

  if (!lib_handle) {
    std::cout << "dlopen() failed: " << dlerror() << '\n';

    std::exit(1);
  }

  void *func = dlsym(lib_handle, "lib");

  if (func) {
    reinterpret_cast<void (*)()>(func)();

  } else {
    const char *err = dlerror();

    if (!err) {
      std::cout << "lib() not found\n";
    } else {
      std::cout << "dlsym() fail: " << err << '\n';
    }
  }

  dlclose(lib_handle);

  return 0;
}
