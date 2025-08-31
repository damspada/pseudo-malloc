#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/my_malloc.h"

// Testing pseudo malloc implementation

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

void test_basic_malloc_free() {
    printf("\n--- Basic malloc/free tests ---\n");
    
    // Try a simple small allocation
    void* ptr = my_malloc(100);
    check(ptr != NULL, "malloc(100) works");

    
    // Try writing to the allocated memory
    if (ptr) {
        strcpy((char*)ptr, "Hello World!");
        check(strcmp((char*)ptr, "Hello World!") == 0, 
                   "can write and read from allocated memory");
        
        // Free it
        my_free(ptr);
    }

}

void test_different_sizes() {
    printf("\n--- Testing different allocation sizes ---\n");
        
    // Test various sizes
    void* tiny = my_malloc(1);
    void* small = my_malloc(64);
    void* medium = my_malloc(512);
    void* large = my_malloc(4096);
    void* huge = my_malloc(8192);  // This should use mmap

    check(tiny != NULL, "1 byte allocation");
    check(small != NULL, "64 byte allocation");
    check(medium != NULL, "512 byte allocation");
    check(large != NULL, "4KB allocation");
    check(huge != NULL, "8KB allocation (should use mmap)");

    // Make sure they're all different
    int all_different = (tiny != small && small != medium && 
                        medium != large && large != huge);
    check(all_different, "all allocations have different addresses");

    // Free them all
    my_free(tiny);
    my_free(small);
    my_free(medium);
    my_free(large);
    my_free(huge);
    
}

void test_multiple_allocations() {
    printf("\n--- Testing multiple allocations ---\n");
    
    // Allocate several blocks
    void* blocks[8];
    // Allocate storage for expected contents to avoid writing into uninitialized pointers
    char block_contents[8][32];
    int successful = 0;
    
    for (int i = 0; i < 8; i++) {
        blocks[i] = my_malloc(128 + i * 16);  // Varying sizes
        if (blocks[i] != NULL) {
            successful++;
            // Write something to each block
            snprintf((char*)blocks[i], sizeof(block_contents[i]), "Block %d", i);
            snprintf(block_contents[i], sizeof(block_contents[i]), "Block %d", i);
        }
    }

    check(successful == 8, "allocated all blocks successfully");

    // Verify the data is still there
    int data_ok = 1;
    for (int i = 0; i < 8; i++) {
        if (blocks[i]) {
            if (strcmp((char*)blocks[i], block_contents[i]) != 0) {
                data_ok = 0;
                break;
            }
        }
    }

    check(data_ok, "data integrity check passed");

    // Free every other block first
    for (int i = 0; i < 8; i += 2) {
        if (blocks[i]) {
            my_free(blocks[i]);
        }
    }
    
    // Then free the rest
    for (int i = 1; i < 8; i += 2) {
        if (blocks[i]) {
            my_free(blocks[i]);
        }
    }
    
}

void test_edge_cases() {
    printf("\n--- Testing edge cases ---\n");
        
    // What happens with malloc(0)?
    void* zero_ptr = my_malloc(0);
    check(zero_ptr == NULL, "malloc(0) returns NULL");
    
    // Free NULL pointer (should be safe)
    my_free(NULL);
    check(1, "free(NULL) doesn't crash");

}

void test_small_allocation_after_free_big_block() {
    printf("\n--- Testing small allocation after freeing a big block ---\n");

    // Allocate a large block
    void* large_block = my_malloc(512);  // 512 bytes
    check(large_block != NULL, "Allocated 512 bytes block");

    // Allocate a small block
    void* small_block = my_malloc(128);
    check(small_block != NULL, "Allocated 128 bytes block");

    // Free the large block
    my_free(large_block);

    // Now allocate another small block
    void* small_block2 = my_malloc(128);

    check(small_block2 != NULL, "Allocated another 128 bytes block");

    // check that the index of the second small block is before the first block
    check(small_block2 < small_block, "Second small block is before first small block");

    // Free the small block
    my_free(small_block2);

    // Free the first small block
    my_free(small_block);

}

void test_full_allocation_of_buddy_pool_512() {
    printf("\n--- Testing full allocation of buddy pool ---\n");

    // Allocate memory until the 1048576 bytes pool is full with 2048 blocks of 512 bytes
    void* blocks[2048];
    for (int i = 0; i < 2048; i++) {
        blocks[i] = my_malloc(512);
    }

    // Try to allocate one more block (should fail)
    void* extra = my_malloc(128);
    check(extra == NULL, "Extra allocation fails when pool is full");

    // Free all blocks
    for (int i = 0; i < 2048; i++) {
        my_free(blocks[i]);
    }

}

void test_full_allocation_of_buddy_pool_1023() {
    printf("\n--- Testing full allocation of buddy pool ---\n");

    // Allocate memory until the 1048576 bytes pool is full with 1024 blocks of 1023 bytes (so rounded to 1024 bit but 1023 to avoid the threshold check)
    void* blocks[1024];
    for (int i = 0; i < 1024; i++) {
        blocks[i] = my_malloc(1023);
    }

    // Try to allocate one more block (should fail)
    void* extra = my_malloc(128);
    check(extra == NULL, "Extra allocation fails when pool is full");

    // Free all blocks
    for (int i = 0; i < 1024; i++) {
        my_free(blocks[i]);
    }
    

}

int main() {

    printf("Running pseudo malloc tests...\n");
    
    test_basic_malloc_free();
    test_different_sizes();
    test_multiple_allocations();
    test_edge_cases();
    test_small_allocation_after_free_big_block();
    test_full_allocation_of_buddy_pool_512();
    test_full_allocation_of_buddy_pool_1023();

    printf("\nResults: %d passed, %d failed\n", passed, failed);
    
    if (failed == 0) {
        printf("All tests passed! 🎉\n");
        return 0;
    } else {
        printf("Some tests failed 😞\n");
        return 1;
    }

}
