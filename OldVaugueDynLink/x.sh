# Demonstrate that vague linkage is not done with a dynamically linked library.

clean ()
{
rm -f *.o lib.a *main
}

showf ()
{
echo ''
echo $1
echo '-----------------'
cat $1
}

sgcc ()
{
echo ''
echo gcc $*
gcc $*
}

exe ()
{
echo ''
echo ./$1
./$1
}

clean

CMN_OPTS='-O2 -Wall -Wextra -pedantic -std=c++11'

# No library, all code in .o files.

showf foo.h

showf lib.cc

showf call2.cc

sgcc $CMN_OPTS -DLIB -c lib.cc call2.cc

showf main.cc

showf call.cc

sgcc $CMN_OPTS -c main.cc call.cc

sgcc $CMN_OPTS main.o call.o lib.o call2.o -lstdc++ -o main

exe main

clean

# With dynamically linked library.

sgcc $CMN_OPTS -DLIB -fPIC -c lib.cc call2.cc

sgcc $CMN_OPTS -shared lib.o call2.o -o lib.a

showf dmain.cc

sgcc $CMN_OPTS -c dmain.cc call.cc

sgcc $CMN_OPTS dmain.o call.o -lstdc++ -ldl -o dmain

exe dmain

clean
