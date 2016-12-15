#!/bin/sh
# This script turns off Linux IP spoofing protection
# so that we can followed encapsulated packets.
# Otherwise, the encapsulated packets will be dropped
# because the source IP address of the packet doesn't match
# the IP address of the sender.
#
# Credit: http://www.linuxtopia.org/online_books/linux_system_administration/securing_and_optimizing_linux/chap5sec60.html
#
for f in /proc/sys/net/ipv4/conf/*/rp_filter; do
    echo 0 > $f
done

