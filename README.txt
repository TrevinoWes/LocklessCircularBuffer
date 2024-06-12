Lockless SPSC Queue inspired by Charles Frasch's CppCon 2023 Talk
Link: https://www.youtube.com/watch?v=K3P_Lmq6pw0

Initial tests using nieve sequentially consistent design average 85 ops/ms

perf report results:

    client:
        sendVal: 74.99%
        readVal: 24.63%
    server:
        Producer Thread:
            socket read: 66.62%
            SPSC push: 31.35%
        Consumer Thread:
            socket write: 4.84%
            SPSC pop: 76.07%

Unsurprisingly socket IO takes a significant amount of the CPU cycles
but suprisingly this is just for client write and server producer read.
The significant wait on consumer pop from the SPSC may be partially from startup so I will
add a warmup wait for procceding tests.

4/10/24
Using scatter-gather APIs with a timeout set on the socket skyrocketed the throughput to
3458.91 ops/ms

4/14/24
using memory_ordering and cache aligning the SPSC index values we were able to jump upto
3636.91 ops/ms

6/12/24
At this point the bottleneck is still the context switching during socket reads/writes.
I was able to see major improvements when removing sanitizing and I would also see a bump
if I were to use a release build instead of debug.