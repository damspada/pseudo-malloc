#include "../include/buddy_allocator.h"

BuddyAllocator* BuddyAllocator_init() {
    
    printf("[BuddyAllocator_init]: Initializing Buddy Allocator\n");

    // Initialize the BuddyAllocator structure
    BuddyAllocator* alloc = malloc(sizeof(BuddyAllocator));
    if (!alloc) {
        fprintf(stderr, "[BuddyAllocator_init]: Memory allocation failed\n");
        return NULL;
    }

    // Allocate the memory pool
    alloc->memory_pool = malloc(MAX_BLOCK_SIZE);
    if (!alloc->memory_pool) {
        fprintf(stderr, "[BuddyAllocator_init]: Memory pool allocation failed\n");
        free(alloc);
        return NULL;
    }

    // Initialize bitmap
    alloc->allocation_bitmap = bitmap_init(MAX_BLOCK_SIZE);

    if (!alloc->allocation_bitmap) {
        fprintf(stderr, "[BuddyAllocator_init]: Bitmap initialization failed\n");
        free(alloc->memory_pool);
        free(alloc);
        return NULL;
    }

    return alloc;

}

void* BuddyAllocator_malloc(BuddyAllocator* allocator, size_t size) {

    printf("[BuddyAllocator_malloc]: Requested size: %zu bytes\n", size);

    // We don't check that size is > 0 because it is already checked in my_malloc.c
    // We don't check that size exceeds MAX_BLOCK_SIZE because it is already checked in my_malloc.c

    // Check if buddy is initialized
    if (!allocator) {
        printf("[BuddyAllocator_malloc]: Buddy Allocator not initialized\n");
        allocator = BuddyAllocator_init();
        
        if (!allocator) {
            fprintf(stderr, "[BuddyAllocator_malloc]: Failed to initialize buddy allocator\n");
            return NULL;
        }
    }

    // Very small allocations have proportionally higher overhead costs so we round up
    size_t aligned_size = size;
    if (aligned_size < MIN_BLOCK_SIZE) {
        printf("[BuddyAllocator_malloc]: Requested size of %zu too small, rounding up to minimum block size %zu\n", size, MIN_BLOCK_SIZE);
        aligned_size = MIN_BLOCK_SIZE;
    }

    // Find the level with blocks large enough for the request
    size_t block_size = MAX_BLOCK_SIZE;
    int current_level = 0;
    while (block_size / 2 >= aligned_size && block_size / 2 >= MIN_BLOCK_SIZE) {
        block_size /= 2;
        current_level++;
    }

    /* ... */

}

void BuddyAllocator_free(BuddyAllocator* allocator, void* ptr) {

    printf("[BuddyAllocator_free]: Freeing pointer %p\n", ptr);

    /* ... */

}