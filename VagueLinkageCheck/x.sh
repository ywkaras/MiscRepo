cat h.h
cat main.cc
cat a.cc
cc -c -O3 -pedantic -Wall -Wextra main.cc
cc -c -O3 -pedantic -Wall -Wextra a.cc
cc -O3 -pedantic -Wall -Wextra main.o a.o -lstdc++
./a.out
rm -f main.o a.o a.out
