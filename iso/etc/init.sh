#!/bin/dash

export PATH=/bin
export SHLVL=0

mount /dev .virtual devfs

echo bonjour

while true; do
  setsid /bin/dash /etc/init2.sh /dev/tty0
done
