#include "../include/buddy_allocator.h"

// Check if any parent node up the tree is marked as allocated
static int any_ancestor_set(const Bitmap* bm, size_t node_index) {
    if (!bm) 
        return 0;

    // Walk up the tree checking each parent
    while (node_index > 0) {

        // Get the parent index
        size_t parent = (node_index - 1) / 2;
        // Ex. 0 -> 0, 1 -> 0, 2 -> 1, 3 -> 1, 4 -> 2, 5 -> 2, 6 -> 3, 7 -> 3

        // Check if this parent is allocated
        if(bitmap_test(bm, parent)){
            return 1;
        }

        // Move up to the next level
        node_index = parent;
    }

    // Arrived at the root without finding any allocated ancestors
    return 0;
}

// Check if any child nodes below this one are allocated
static int any_descendant_set(const Bitmap* bm, size_t node_index, int node_level) {
    if (!bm) 
        return 0;
    
    int max_depth = MAX_LEVELS;

    // Calculate how many levels are below the current node
    int levels_below = (max_depth - 1) - node_level;
    // Ex. node level = 2, levels below = (14 - 1) - 2 = 11

    // Check each level below the current node
    for (int level = 1; level <= levels_below; level++) {

        // Calculate the number of children at this level
        size_t num_children = ((size_t)1 << level); // 2^level

        // Convert to 1-based (to have and easy formula to find the first child)
        size_t node_index_1based = (node_index + 1);

        // Calculate the 1-based index of the first child at this level
        size_t first_child_1based = node_index_1based * num_children;

        // Convert to 0-based
        size_t first_child = first_child_1based - 1;
        
        // Check each child at this level
        for (size_t i = 0; i < num_children; i++) {
            if (bitmap_test(bm, first_child + i)) {
                return 1;
            }
        }
    }
    
    // Checked all descendants, none are allocated
    return 0;
}

BuddyAllocator* BuddyAllocator_init(BuddyAllocator* allocator) {
    
    printf("[BuddyAllocator_init]: Initializing Buddy Allocator\n");

    // If allocator is NULL, create a new one
    if (!allocator) {
        allocator = malloc(sizeof(BuddyAllocator));
        if (!allocator) {
            fprintf(stderr, "[BuddyAllocator_init]: Error: Memory allocation failed\n");
            return NULL;
        }

        // Initialize pointers to NULL for new allocator
        allocator->memory_pool = NULL;
        allocator->allocation_bitmap = NULL;
    }

    // Check if memory pool needs allocation
    if (!allocator->memory_pool) {
        allocator->memory_pool = malloc(MAX_BLOCK_SIZE);
        if (!allocator->memory_pool) {
            fprintf(stderr, "[BuddyAllocator_init]: Error: Memory pool allocation failed\n");
            if (allocator->allocation_bitmap) {
                free(allocator);
            }
            return NULL;
        }
    }

    // Check if bitmap needs initialization
    if (!allocator->allocation_bitmap) {
        allocator->allocation_bitmap = bitmap_init(MAX_BLOCK_SIZE);
        if (!allocator->allocation_bitmap) {
            fprintf(stderr, "[BuddyAllocator_init]: Error: Bitmap initialization failed\n");
            free(allocator->memory_pool);
            free(allocator);
            return NULL;
        }
    }

    return allocator;
}

void* BuddyAllocator_malloc(BuddyAllocator* allocator, size_t size) {

    printf("[BuddyAllocator_malloc]: Requested size: %zu bytes\n", size);

    // Check that size is != 0 (already checked in my_malloc.c)
    if (size == 0) {
        printf("[BuddyAllocator_malloc]: Warning: Requested size is 0\n");
        return NULL;
    }

    // Check that size exceeds MAX_BLOCK_SIZE (already checked in my_malloc.c)
    if (size > MAX_BLOCK_SIZE) {
        fprintf(stderr, "[BuddyAllocator_malloc]: Error: Requested size exceeds maximum block size\n");
        return NULL;
    }

    // Check if buddy allocator struct is initialized
    if (!allocator || !allocator->memory_pool || !allocator->allocation_bitmap) {
        printf("[BuddyAllocator_malloc]: Buddy Allocator not initialized\n");
        allocator = BuddyAllocator_init(allocator);
        if (!allocator) {
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

        // Skip if this block is already allocated
        if (bitmap_test(bitmap, bitmap_index) == 1) {
            continue;
        }

        // Skip if any parent block is allocated (would overlap)
        if (any_ancestor_set(bitmap, bitmap_index)) {
            continue;
        }

        // Skip if any child blocks are allocated (would fragment)
        if (any_descendant_set(bitmap, bitmap_index, current_level)) {
            continue;
        }

        // Found a usable block
        level_found = current_level;
        index_found = i;
        found = 1;
        break;
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

    printf("[BuddyAllocator_malloc]: Allocated block at level %d, index %d, address %p, size %zu\n", level_found, index_found, allocated_block, block_size);

    return allocated_block;
    
}

void* BuddyAllocator_malloc_metabuddy(BuddyAllocator* allocator, size_t size) {

    printf("[BuddyAllocator_malloc_metabuddy]: Requested size: %zu bytes\n", size);

    // Check that size is != 0 (already checked in my_malloc.c)
    if (size == 0) {
        printf("[BuddyAllocator_malloc_metabuddy]: Warning: Requested size is 0\n");
        return NULL;
    }

    // Check that size exceeds MAX_BLOCK_SIZE (already checked in my_malloc.c)
    if (size > MAX_BLOCK_SIZE) {
        fprintf(stderr, "[BuddyAllocator_malloc_metabuddy]: Error: Requested size exceeds maximum block size\n");
        return NULL;
    }

    // Check if buddy allocator struct is initialized
    if (!allocator || !allocator->memory_pool || !allocator->allocation_bitmap) {
        printf("[BuddyAllocator_malloc_metabuddy]: Buddy Allocator not initialized\n");
        allocator = BuddyAllocator_init(allocator);
        if (!allocator) {
            return NULL;
        }
    }

    // Size with metadata
    size_t size_with_metadata = size + sizeof(size_t);

    // Very small allocations have proportionally higher overhead costs so we round up
    size_t aligned_size = size_with_metadata;
    if (aligned_size < MIN_BLOCK_SIZE) {
        printf("[BuddyAllocator_malloc_metabuddy]: Requested size of %zu too small, rounding up to minimum block size %d\n", size, MIN_BLOCK_SIZE);
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

        // Skip if this block is already allocated
        if (bitmap_test(bitmap, bitmap_index) == 1) {
            continue;
        }

        // Skip if any parent block is allocated (would overlap)
        if (any_ancestor_set(bitmap, bitmap_index)) {
            continue;
        }

        // Skip if any child blocks are allocated (would fragment)
        if (any_descendant_set(bitmap, bitmap_index, current_level)) {
            continue;
        }

        // Found a usable block
        level_found = current_level;
        index_found = i;
        found = 1;
        break;
    }

    if (!found) {
        fprintf(stderr, "[BuddyAllocator_malloc_metabuddy]: Error: No free block found at level %d\n", current_level);
        return NULL;
    }

    // Mark the block as allocated in the bitmap
    size_t allocated_index = start_index + index_found;
    bitmap_set(allocator->allocation_bitmap, allocated_index);

    // Calculate the memory address of the allocated block
    size_t offset = index_found * block_size;
    void* allocated_block = (void*)((unsigned long)allocator->memory_pool + offset);

    // Store bitmap index for easy freeing
    size_t* metadata = (size_t*)allocated_block;
    *metadata = allocated_index;

    // Move pointer to the location just after the metadata
    void * allocated_block_ptr = (void*)((char*)allocated_block + sizeof(size_t));

    printf("[BuddyAllocator_malloc_metabuddy]: Allocated block at level %d, index %d, address %p, size %zu (including %zu bytes of metadata)\n", 
           level_found, index_found, allocated_block, block_size, sizeof(size_t));

    return allocated_block_ptr;
    
}

void BuddyAllocator_free(BuddyAllocator* allocator, void* ptr) {

    if (ptr == NULL) {
        printf("[BuddyAllocator_free]: Warning: Attempting to free NULL pointer, ignoring\n");
        return;
    }

    printf("[BuddyAllocator_free]: Freeing pointer %p\n", ptr);

    // Check if buddy is initialized
    if (!allocator || !allocator->memory_pool || !allocator->allocation_bitmap) {
        printf("[BuddyAllocator_free]: Error: Buddy Allocator not properly initialized\n");
        return;
    }

    // Compute pool bounds
    char* pool_start = (char*)allocator->memory_pool;
    char* pool_end = pool_start + MAX_BLOCK_SIZE;

    // Check if pointer is within the memory pool bounds
    if ((char*)ptr < pool_start || (char*)ptr >= pool_end) {
        fprintf(stderr, "[BuddyAllocator_free]: Error: Pointer %p is outside memory pool bounds\n", ptr);
        return;
    }

    // Calculate offset from the start of memory pool
    size_t offset = (char*)ptr - pool_start;
    
    // Find which level this block belongs to by checking all possible levels
    int found_level = -1;
    size_t found_index = 0;
    size_t found_block_size = 0;
    size_t found_bitmap_index = 0;
    
    // Start from the smallest blocks
    for (int level = 0; level < MAX_LEVELS; level++) {
        size_t block_size = MAX_BLOCK_SIZE / (1 << level); // is like doing MAX_BLOCK_SIZE / (2^level)

        // Check if this block size is valid (not smaller than minimum)
        if (block_size < MIN_BLOCK_SIZE) {
            break;
        }
        
        // Check if the offset is aligned to this block size
        if (offset % block_size == 0) {
            size_t block_index = offset / block_size;
            size_t start_index = ((size_t)1 << level) - 1; // First block index at this level
            size_t bitmap_index = start_index + block_index;
            
            // Check if this block is actually allocated in the bitmap
            if (bitmap_test(allocator->allocation_bitmap, bitmap_index) == 1) {
                found_level = level;
                found_index = block_index;
                found_block_size = block_size;
                found_bitmap_index = bitmap_index;
                break;
            }
        }
    }
    
    if (found_level == -1) {
        fprintf(stderr, "[BuddyAllocator_free]: Error: Could not find allocated block for pointer %p\n", ptr);
        return;
    }
    
    // Clear the stored bitmap index
    bitmap_clear(allocator->allocation_bitmap, found_bitmap_index);

    printf("[BuddyAllocator_free]: Freed block at level %d, index %zu, size %zu bytes\n", found_level, found_index, found_block_size);

}


void BuddyAllocator_free_metabuddy(BuddyAllocator* allocator, void* ptr) {

    if (ptr == NULL) {
        printf("[BuddyAllocator_free_metabuddy]: Warning: Attempting to free NULL pointer, ignoring\n");
        return;
    }

    printf("[BuddyAllocator_free_metabuddy]: Freeing pointer %p\n", ptr);

    // Check if buddy is initialized
    if (!allocator || !allocator->memory_pool || !allocator->allocation_bitmap) {
        printf("[BuddyAllocator_free_metabuddy]: Error: Buddy Allocator not properly initialized\n");
        return;
    }

    // Compute pool bounds
    char* pool_start = (char*)allocator->memory_pool;
    char* pool_end = pool_start + MAX_BLOCK_SIZE;

    // Check if pointer is within the memory pool bounds
    if ((char*)ptr < pool_start || (char*)ptr >= pool_end) {
        fprintf(stderr, "[BuddyAllocator_free_metabuddy]: Error: Pointer %p is outside memory pool bounds\n", ptr);
        return;
    }

    // Retrieve metadata so the index of the block in the bitmap
    size_t* metadata = (size_t*)((char*)ptr - sizeof(size_t));
    size_t found_bitmap_index = *metadata;

    // Clear the stored bitmap index
    bitmap_clear(allocator->allocation_bitmap, found_bitmap_index);

    printf("[BuddyAllocator_free_metabuddy]: Freed block of bitmap index %zu\n", found_bitmap_index);

}