#define _GNU_SOURCE
#include "../include/my_malloc.h"

BuddyAllocator buddy_allocator = {0};

// Round up to page
static inline size_t round_to_pages(size_t size) {
    size_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    return num_pages * PAGE_SIZE;
}

void* my_malloc(size_t size) {

    // Since size_t is unsigned long is always >= 0 is unnecessary check if < 0

    // Zero size --> return NULL
    if(size == 0) {
        printf("[my_malloc]: Warning: size is 0, returning NULL\n");
        return NULL;
    }

    // Small size --> buddy allocator
    if (size < SMALL_THRESHOLD) {
        printf("[my_malloc]: Small size (%zu), using BuddyAllocator\n", size);
        void* ptr = BuddyAllocator_malloc(&buddy_allocator, size);
        if (!ptr) {
            fprintf(stderr, "[my_malloc]: Error: BuddyAllocator failed\n");
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
        fprintf(stderr, "[my_malloc]: Error: mmap failed\n");
        return NULL;
    }

    // Store metadata
    size_t* metadata = (size_t*)ptr;
    *metadata = size;  // Store original requested size

    // Return pointer after metadata
    void* return_ptr = (char*)ptr + sizeof(size_t);
    printf("[my_malloc]: Successful allocation: ptr=%p, requested=%zu, allocated=%zu\n", return_ptr, size, alloc_size);

    return return_ptr;
}

void my_free(void* ptr) {

    // Check if ptr is NULL
    if (ptr == NULL) {
        printf("[my_free]: Warning: attempting to free NULL pointer\n");
        return;
    }

    // Check if ptr is within BuddyAllocator range --> ptr deallocation with BuddyAllocator
    if (ptr >= buddy_allocator.memory_pool && ptr < (buddy_allocator.memory_pool + BUDDY_POOL_SIZE)) {
        printf("[my_free]: Pointer deallocation using BuddyAllocator free..\n");
        BuddyAllocator_free(&buddy_allocator, ptr);
        return;
    }

    // Ptr deallocation with munmap
    printf("[my_free]: Pointer deallocation with munmap..\n");
    void* metadata_ptr = (char*)ptr - sizeof(size_t);

    // Get the original requested size stored in metadata
    size_t requested_size = *(size_t*)metadata_ptr;

    // Calculate the total allocated size (rounded to page size)
    size_t alloc_size = round_to_pages(sizeof(size_t) + requested_size);
    
    if (munmap(metadata_ptr, alloc_size) == -1) {
        fprintf(stderr, "[my_free]: Error: munmap failed\n");
        return;
    }
    
    printf("[my_free]: Successfully freed %zu bytes (requested: %zu)\n", alloc_size, requested_size);
}