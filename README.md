# pseudo-malloc

implements a custom memory allocator, that replaces the standard malloc function, **using mmap to obtain physical memory from the operating system, while managing allocation requests through a custom logic based on request size.**

### Allocation Strategy:
- _Small requests_: **Up to 1/4 of a page size (4 KB / 4 = 1 KB) → Uses buddy allocator with a total pool of 1 MB**
- _Large requests_: **1 KB and above → Uses direct mmap allocation**

### Objective:
**Reduce mmap system calls** by limiting their use only to significantly large memory requests, while efficiently managing smaller allocations through a pre-allocated buddy system pool.
