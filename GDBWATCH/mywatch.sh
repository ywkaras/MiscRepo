#!/bin/bash

gcc -g -O3 x.cc -lstdc++ -lpthread

rm -f log.out /tmp/xx

./a.out &

( sleep 3 ; touch /tmp/xx ) &

TS_PID=$( ps -ef | fgrep -v grep | fgrep a.out | sed 's/  */\n/g' | head -2 | tail -1 )
cat << STOP | gdb a.out $TS_PID # 2>&1
set pagination off
set logging file log.out
set logging on
handle SIGPIPE noprint nostop pass
watch f
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

