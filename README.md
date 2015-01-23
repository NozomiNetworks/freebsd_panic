# panic
A panic kld module for FreeBSD.

## How to use:

1. make
2. kldload ./panic.ko
3. sysctl panic to see tunable variables
4. set timewait(in seconds) & method
   vaild method: direct, null, random, kdb
5. sysctl panic.enter=1
