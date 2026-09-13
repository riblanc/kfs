#!/bin/dash

exec <$1 >>$1 2>>$1
stty icanon echo echoctl

echo $1 "(pid: $$)"

exec dash