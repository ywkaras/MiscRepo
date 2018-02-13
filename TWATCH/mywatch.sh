#!/bin/bash

# NOTE:  Must be root to run this.  Probably will have to run 'echo -1 > /proc/sys/kernel/sched_rt_runtime_us'
#        for the pthread_setschedparam() to work.

rm -f log.out /tmp/xx watch_test

GCC=/opt/rh/devtoolset-8/root/bin/x86_64-redhat-linux-gcc-8

$GCC -g -O3 -std=c++17 x.cc -o watch_test -lstdc++ -lpthread

#nice --adjustment=-20 ./watch_test &
./watch_test &

( sleep 3 ; touch /tmp/xx ) &

WATCH_TEST_PID=$( ps -ef | fgrep -v grep | fgrep watch_test | sed 's/  */\n/g' | head -2 | tail -1 )
cat << STOP | gdb watch_test $WATCH_TEST_PID
set pagination off
set logging file log.out
set logging on
handle SIGPIPE noprint nostop pass
break break_on
commands
thread apply all bt full
q
y
end
c
thread apply all bt full
q
y
STOP
kill $WATCH_TEST_PID
rm -f /tmp/xx
