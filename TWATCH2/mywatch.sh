#!/bin/bash

# NOTE:  Must be root to run this.  Probably will have to run 'echo -1 > /proc/sys/kernel/sched_rt_runtime_us'
#        for the pthread_setschedparam() to work.

rm -f log.out watch_test

GCC=/opt/rh/devtoolset-8/root/bin/x86_64-redhat-linux-gcc-8

$GCC -g -O3 -std=c++17 x.cc -o watch_test -lstdc++ -lpthread

./watch_test > log.out

rm -f watch_test
