#!/bin/bash

rm -f log.out /tmp/xx watch_test

gcc -g -O3 -std=c++17 x.cc -o watch_test -lstdc++ -lpthread

./watch_test &

( sleep 3 ; touch /tmp/xx ) &

WATCH_TEST_PID=$( ps -ef | fgrep -v grep | fgrep watch_test | sed 's/  */\n/g' | head -2 | tail -1 )
cat << STOP | gdb watch_test $WATCH_TEST_PID
set pagination off
set logging file log.out
set logging on
handle SIGPIPE noprint nostop pass
watch -location f
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
