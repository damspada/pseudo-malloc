#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include "bitmap.h"
#include "my_malloc.h"

#include <stdlib.h>

#define MIN_BLOCK_SIZE 64 // Smallest block size in bytes
#define MAX_BLOCK_SIZE BUDDY_POOL_SIZE // Largest block size is the size of the buddy memory pool so 1MB
#define MAX_LEVELS (log2(MAX_BLOCK_SIZE) - log2(MIN_BLOCK_SIZE) + 1) // Maximum number of levels

// Buddy Allocator Structure
typedef struct {
    void* memory_pool; // Pointer to the entire memory pool
    Bitmap* allocation_bitmap; // Tracks which blocks are allocated
} BuddyAllocator;

// Initialize the Buddy Allocator
BuddyAllocator* BuddyAllocator_init();

// Allocate memory using the Buddy Allocator
void* BuddyAllocator_malloc(BuddyAllocator* allocator, size_t size);

// Free memory using the Buddy Allocator
void BuddyAllocator_free(BuddyAllocator* allocator, void* ptr);

#endif
