rm -f ./a.out *.o *.so
CMN="-std=c++11 -pedantic -Wall -Wextra -O3 -DINLINE_FUNC_SINGLETON=$INLINE_FUNC_SINGLETON"
DLL_EXTRA=""
if [[ $(uname) = Darwin ]] ; then
    # MacOS
    DLL_EXTRA="-flat_namespace -undefined suppress"
fi
g++ $CMN $DLL_EXTRA -shared -fpic -o lib.so lib.cc
g++ $CMN -c -o main.o main.cc
g++ main.o lib.so -lstdc++
export LD_LIBRARY_PATH=.
./a.out
rm -f ./a.out *.o *.so
