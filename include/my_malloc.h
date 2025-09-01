#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include "buddy_allocator.h"

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define PAGE_SIZE 4096 // 4KB
#define SMALL_THRESHOLD (PAGE_SIZE / 4) // 1024 bytes so 1KB
#define BUDDY_POOL_SIZE (1024 * 1024)  // 1MB (1024 * 1024 = 1048576 bytes so 1MB) for buddy allocator

void* my_malloc(size_t size); // Allocate memory
void my_free(void* ptr); // Free memory

void* my_malloc_metabuddy(size_t size); // Allocate memory with metabuddy
void my_free_metabuddy(void* ptr); // Free memory with metabuddy


#endif // MY_MALLOC_H