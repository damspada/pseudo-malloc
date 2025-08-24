#include "../include/buddy_allocator.h"

BuddyAllocator buddy_allocator;

BuddyAllocator* BuddyAllocator_init() {
    
    printf("[BuddyAllocator_init]: Initializing Buddy Allocator\n");

    // Initialize the BuddyAllocator structure
    BuddyAllocator* alloc = malloc(sizeof(BuddyAllocator));
    if (!alloc) {
        fprintf(stderr, "[BuddyAllocator_init]: Error: Memory allocation failed\n");
        return NULL;
    }

    // Allocate the memory pool
    alloc->memory_pool = malloc(MAX_BLOCK_SIZE);
    if (!alloc->memory_pool) {
        fprintf(stderr, "[BuddyAllocator_init]: Error: Memory pool allocation failed\n");
        free(alloc);
        return NULL;
    }

    // Initialize bitmap
    alloc->allocation_bitmap = bitmap_init(MAX_BLOCK_SIZE);

    if (!alloc->allocation_bitmap) {
        fprintf(stderr, "[BuddyAllocator_init]: Error: Bitmap initialization failed\n");
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
        printf("[BuddyAllocator_malloc]:Buddy Allocator not initialized\n");
        allocator = BuddyAllocator_init();
        
        if (!allocator) {
            fprintf(stderr, "[BuddyAllocator_malloc]: Error: Failed to initialize buddy allocator\n");
            return NULL;
        }
    }

    // Very small allocations have proportionally higher overhead costs so we round up
    size_t aligned_size = size;
    if (aligned_size < MIN_BLOCK_SIZE) {
        printf("[BuddyAllocator_malloc]: Requested size of %zu too small, rounding up to minimum block size %d\n", size, MIN_BLOCK_SIZE);
        aligned_size = MIN_BLOCK_SIZE;
    }

    // Find the level with blocks large enough for the request
    size_t block_size = MAX_BLOCK_SIZE;
    int current_level = 0;
    while (block_size / 2 >= aligned_size && block_size / 2 >= MIN_BLOCK_SIZE) {
        block_size /= 2;
        current_level++;
    }

    // Variables to store the found block information
    int level_found = -1;
    int index_found = -1;
    int found = 0;
    
    // Try to find a free block at this level
    Bitmap* bitmap = allocator->allocation_bitmap;
    size_t blocks_at_level = MAX_BLOCK_SIZE / block_size; // Number of blocks at this level
    size_t start_index = (1 << current_level) - 1; // The index of the first block at this level in the bitmap using 2^level - 1

    for (size_t i = 0; i < blocks_at_level; i++) {
        size_t bitmap_index = start_index + i;

        // Check if the block is free
        if (bitmap_test(bitmap, bitmap_index) == 0) {  // 0 means bit is not set (block is free)
            level_found = current_level;
            index_found = i;
            found = 1;
            break;
        }
    }

    if (!found) {
        fprintf(stderr, "[BuddyAllocator_malloc]: Error: No free block found at level %d\n", current_level);
        return NULL;
    }

    // Mark the block as allocated in the bitmap
    size_t allocated_index = start_index + index_found;
    bitmap_set(allocator->allocation_bitmap, allocated_index);

    // Calculate the memory address of the allocated block
    size_t offset = index_found * block_size;
    void* allocated_block = (void*)((unsigned long)allocator->memory_pool + offset);

    printf("[BuddyAllocator_malloc]: Allocated block at level %d, index %d, address %p\n", level_found, index_found, allocated_block);

    return allocated_block;
    
}

void BuddyAllocator_free(BuddyAllocator* allocator, void* ptr) {

    printf("[BuddyAllocator_free]: Freeing pointer %p\n", ptr);

    // Check if buddy is initialized
    if (!allocator) {
        printf("[BuddyAllocator_free]: Error: Buddy Allocator not initialized\n");
        return;
    }

    // Find the index in the bitmap corresponding to the pointer
    size_t index = (size_t)ptr - (size_t)allocator->memory_pool;

    if(index >= MAX_BLOCK_SIZE) {
        printf("[BuddyAllocator_free]: Error: Pointer %p out of bounds\n", ptr);
        return;
    }

    if(index < MIN_BLOCK_SIZE) {
        printf("[BuddyAllocator_free]: Error: Pointer %p is above first possible block\n", ptr);
        return;
    }

    if (index % MIN_BLOCK_SIZE != 0) {
        printf("[BuddyAllocator_free]: Error: Invalid pointer %p\n", ptr);
        return;
    }

    // Find the block index in the bitmap
    size_t block_index = index / MIN_BLOCK_SIZE;
    // ex. index = 51 -> block_index = 51 / 16 = 3

    bitmap_clear(allocator->allocation_bitmap, block_index);
    
    printf("[BuddyAllocator_free]: Freed block at index %zu\n", block_index);

}