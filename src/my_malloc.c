#include "../include/my_malloc.h"
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

extern BuddyAllocator buddy_allocator;

// Check if pointer is page-aligned
static inline int is_page_aligned(void* ptr) {
    return ((uintptr_t)ptr & (PAGE_SIZE - 1)) == 0;
}

// Round up to page size using bit operations
static inline size_t round_to_pages(size_t size) {
    return (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
}

void* my_malloc(size_t size) {

    // Since size_t is always >= 0 is unnecessary check if < 0

    // zero size --> return NULL
    if(size == 0) {
        printf("[my_malloc]: Warning: size is 0, returning NULL\n");
        return NULL;
    }

    // Small size --> buddy allocator
    if (size < SMALL_THRESHOLD) {
        printf("[my_malloc]: Small size (%zu), using BuddyAllocator\n", size);
        void* ptr = BuddyAllocator_malloc(&buddy_allocator, size);
        if (!ptr) {
            printf("[my_malloc]: BuddyAllocator failed\n");
        }
        return ptr;
    }

    // Large size --> use mmap
    printf("[my_malloc]: Large size (%zu), using mmap\n", size);

    // Calculate total size needed (including metadata)
    size_t total_size = size + sizeof(size_t);
    size_t alloc_size = round_to_pages(total_size);

    void* ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // check for correct allocation
    if (ptr == MAP_FAILED) {
        errno = ENOMEM; // Out of memory
        perror("[my_malloc]: ERROR: mmap failed");
        return NULL;
    }

    // Store metadata
    size_t* metadata = (size_t*)ptr;
    *metadata = size;  // Store original requested size

    // Return pointer after metadata
    void* user_ptr = (char*)ptr + sizeof(size_t);
    printf("[my_malloc]: Success: ptr=%p, requested=%zu, allocated=%zu\n", user_ptr, size, alloc_size);
    
    return user_ptr;
}

void my_free(void* ptr) {

    // Check if ptr is NULL
    if (ptr == NULL) {
        printf("[my_free]: Warning: attempting to free NULL pointer\n");
        return;
    }

    // Check if ptr is within BuddyAllocator range ptr is within BuddyAllocator range --> ptr deallocation with BuddyAllocator
    if (ptr >= buddy_allocator.memory && ptr < (buddy_allocator.memory + BUDDY_POOL_SIZE)) {
        printf("[my_free]: Pointer deallocation using BuddyAllocator free..\n");
        BuddyAllocator_free(&buddy_allocator, ptr);
        return;
    }

    // ptr deallocation with munmap
    printf("[my_free]: Pointer deallocation with munmap..\n");
    size_t* metadata = (size_t*)((char*)ptr - sizeof(size_t));

    // Calculate allocation size and check alignment
    size_t alloc_size = round_to_pages(*metadata + sizeof(size_t));
    
    if (munmap(metadata, alloc_size) == -1) {
        perror("[my_free]: Error: munmap failed");
        return;
    }

    printf("[my_free]: Successfully freed %zu bytes\n", alloc_size);
}