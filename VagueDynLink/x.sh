cat foo.h

cat lib.cc

cat main.cc

CMN_OPTS='-O2 -Wall -Wextra -pedantic -std=c++17 -fno-strict-aliasing -fwrapv'

cc $CMN_OPTS -DLIB -fPIC -c lib.cc

cc $CMN_OPTS -shared lib.o -o lib.a

cc $CMN_OPTS -c main.cc

cc $CMN_OPTS main.o -lstdc++ -ldl -o main

./main

rm -f *.o lib.a *main
