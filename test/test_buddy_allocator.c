#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/buddy_allocator.h"

// Testing the buddy allocator (hope it works)

int passed = 0;
int failed = 0;

void check(int condition, const char* msg) {
    if (condition) {
        printf("✓ %s\n", msg);
        passed++;
    } else {
        printf("✗ %s\n", msg);
        failed++;
    }
}

void cleanup_allocator(BuddyAllocator* allocator) {
    if (allocator) {
        if (allocator->allocation_bitmap) bitmap_free(allocator->allocation_bitmap);
        if (allocator->memory_pool) free(allocator->memory_pool);
        free(allocator);
    }
}

void test_initialization() {

    printf("\n--- Testing allocator setup ---\n");
    
    BuddyAllocator* allocator = BuddyAllocator_init(NULL);

    check(allocator != NULL, "allocator created successfully");
    check(allocator->memory_pool != NULL, "memory pool is allocated");
    check(allocator->allocation_bitmap != NULL, "bitmap is set up");

    cleanup_allocator(allocator);

}

void test_simple_allocations() {
    printf("\n--- Testing basic allocations ---\n");
    
    BuddyAllocator* allocator = BuddyAllocator_init(NULL);
    if (!allocator) {
        printf("Failed to create allocator for this test\n");
        return;
    }
    
    // Try allocating some memory
    void* ptr1 = BuddyAllocator_malloc(allocator, 100);
    check(ptr1 != NULL, "allocated 100 bytes");
    
    void* ptr2 = BuddyAllocator_malloc(allocator, 256);
    check(ptr2 != NULL, "allocated 256 bytes");
    
    void* ptr3 = BuddyAllocator_malloc(allocator, 50);
    check(ptr3 != NULL, "allocated 50 bytes");

    // Make sure we got different addresses
    check(ptr1 != ptr2 && ptr2 != ptr3 && ptr1 != ptr3, "all pointers are different");

    // Let's try writing to the memory to make sure it's usable
    if (ptr1) {
        strcpy((char*)ptr1, "Hello");
        check(strcmp((char*)ptr1, "Hello") == 0, "can write to allocated memory");
    }
    
    // Free everything
    BuddyAllocator_free(allocator, ptr1);
    BuddyAllocator_free(allocator, ptr2);
    BuddyAllocator_free(allocator, ptr3);

    // Clean up
    cleanup_allocator(allocator);

}

void test_allocation_patterns() {
    printf("\n--- Testing allocation patterns ---\n");
    
    BuddyAllocator* allocator = BuddyAllocator_init(NULL);
    
    // Allocate a bunch of small blocks
    void* small_blocks[10];
    int successful_allocs = 0;
    
    for (int i = 0; i < 10; i++) {
        small_blocks[i] = BuddyAllocator_malloc(allocator, 64);
        if (small_blocks[i] != NULL) {
            successful_allocs++;
        }
    }

    check(successful_allocs == 10, "managed to allocate multiple small blocks");

    // Free half of them but in the reverse order
    for (int i = 4; i >= 0; i--) {
        if (small_blocks[i]) {
            BuddyAllocator_free(allocator, small_blocks[i]);
        }
    }
    
    // Try to allocate a bigger block
    void* big_block = BuddyAllocator_malloc(allocator, 512);
    check(big_block != NULL, "allocated larger block after freeing smaller ones");

    // Clean up remaining blocks in reverse order
    for (int i = 9; i >= 5; i--) {
        if (small_blocks[i]) {
            BuddyAllocator_free(allocator, small_blocks[i]);
        }
    }

    if (big_block) {
        BuddyAllocator_free(allocator, big_block);
    }
    
    // Clean up allocator
    cleanup_allocator(allocator);

}

void test_edge_cases() {
    printf("\n--- Testing edge cases ---\n");
    
    BuddyAllocator* allocator = BuddyAllocator_init(NULL);
    if (!allocator) return;
    
    // Try allocating 0 bytes - what should happen?
    void* zero_ptr = BuddyAllocator_malloc(allocator, 0);
    check(zero_ptr == NULL, "allocating 0 bytes returns NULL");

    // Try a really large allocation
    void* huge_ptr = BuddyAllocator_malloc(allocator, 1024 * 1024 * 2); // 2MB
    check(huge_ptr == NULL, "huge allocation fails and returns NULL");

    // Try freeing NULL pointer (shouldn't crash)
    BuddyAllocator_free(allocator, NULL);
    check(1, "freeing NULL doesn't crash");

    cleanup_allocator(allocator);
    
}

int main() {

    printf("Running buddy allocator tests...\n");

    test_initialization();
    test_simple_allocations();
    test_allocation_patterns();
    test_edge_cases();
    
    printf("\nResults: %d passed, %d failed\n", passed, failed);
    
    if (failed == 0) {
        printf("All tests passed! 🎉\n");
        return 0;
    } else {
        printf("Some tests failed 😞\n");
        return 1;
    }

}
