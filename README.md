# panic
A panic kld module for FreeBSD.

## How to use:

1. make
2. kldload ./panic.ko
3. sysctl panic: to see tunable variables
4. sysctl panic.timewait=[time]: to set waiting time before panic in seconds
5. sysctl panic.method=[method]: method can be "direct", "null", "random", or "kdb"
6. sysctl panic.enter=1
