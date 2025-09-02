#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/my_malloc.h"
#include "../include/debug_print.h"

// Testing pseudo malloc implementation

int passed = 0;
int failed = 0;

void check(int condition, const char* msg) {
    if (condition) {
        DEBUG_PRINTF("✓ %s\n", msg);
        passed++;
    } else {
        DEBUG_PRINTF("✗ %s\n", msg);
        failed++;
    }
}

void test_basic_malloc_free() {
    DEBUG_PRINTF("\n--- Basic malloc/free tests ---\n");
    
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
    DEBUG_PRINTF("\n--- Testing different allocation sizes ---\n");
        
    // Test various sizes
    void* tiny = my_malloc(1);
    void* small = my_malloc(64);
    void* medium = my_malloc(512);
    void* large = my_malloc(4096); // This should use mmap
    void* huge = my_malloc(8192);  // This should use mmap

    check(tiny != NULL, "1 byte allocation");
    check(small != NULL, "64 byte allocation");
    check(medium != NULL, "512 byte allocation");
    check(large != NULL, "4KB allocation (should use mmap)");
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
    DEBUG_PRINTF("\n--- Testing multiple allocations ---\n");
    
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
    DEBUG_PRINTF("\n--- Testing edge cases ---\n");
        
    // What happens with malloc(0)?
    void* zero_ptr = my_malloc(0);
    check(zero_ptr == NULL, "malloc(0) returns NULL");
    
    // Free NULL pointer (should be safe)
    my_free(NULL);
    check(1, "free(NULL) doesn't crash");

}

void test_small_allocation_after_free_big_block() {
    DEBUG_PRINTF("\n--- Testing small allocation after freeing a big block ---\n");

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
    DEBUG_PRINTF("\n--- Testing full allocation of buddy pool ---\n");

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
    DEBUG_PRINTF("\n--- Testing full allocation of buddy pool ---\n");

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

/* Metabuddy tests */

void test_basic_malloc_free_metabuddy() {
    DEBUG_PRINTF("\n--- Basic malloc/free tests ---\n");
    
    // Try a simple small allocation
    void* ptr = my_malloc_metabuddy(100);
    check(ptr != NULL, "malloc(100) works");

        
    // Try writing to the allocated memory
    if (ptr) {
        strcpy((char*)ptr, "Hello World!");
        check(strcmp((char*)ptr, "Hello World!") == 0, 
                   "can write and read from allocated memory");
        
        // Free it
        my_free_metabuddy(ptr);
    }
}

void test_different_sizes_metabuddy() {
    DEBUG_PRINTF("\n--- Testing different allocation sizes ---\n");
        
    // Test various sizes
    void* tiny = my_malloc_metabuddy(1);
    void* small = my_malloc_metabuddy(64);
    void* medium = my_malloc_metabuddy(512);
    void* large = my_malloc_metabuddy(4096); // This should use mmap
    void* huge = my_malloc_metabuddy(8192);  // This should use mmap

    check(tiny != NULL, "1 byte allocation");
    check(small != NULL, "64 byte allocation");
    check(medium != NULL, "512 byte allocation");
    check(large != NULL, "4KB allocation (should use mmap)");
    check(huge != NULL, "8KB allocation (should use mmap)");

    // Make sure they're all different
    int all_different = (tiny != small && small != medium && 
                        medium != large && large != huge);
    check(all_different, "all allocations have different addresses");

    // Free them all
    my_free_metabuddy(tiny);
    my_free_metabuddy(small);
    my_free_metabuddy(medium);
    my_free_metabuddy(large);
    my_free_metabuddy(huge);
    
}

void test_multiple_allocations_metabuddy() {
    DEBUG_PRINTF("\n--- Testing multiple allocations ---\n");
    
    // Allocate several blocks
    void* blocks[8];
    // Allocate storage for expected contents to avoid writing into uninitialized pointers
    char block_contents[8][32];
    int successful = 0;
    
    for (int i = 0; i < 8; i++) {
        blocks[i] = my_malloc_metabuddy(128 + i * 16);  // Varying sizes
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
            my_free_metabuddy(blocks[i]);
        }
    }
    
    // Then free the rest
    for (int i = 1; i < 8; i += 2) {
        if (blocks[i]) {
            my_free_metabuddy(blocks[i]);
        }
    }
    
}

void test_edge_cases_metabuddy() {
    DEBUG_PRINTF("\n--- Testing edge cases ---\n");
        
    // What happens with malloc(0)?
    void* zero_ptr = my_malloc_metabuddy(0);
    check(zero_ptr == NULL, "malloc(0) returns NULL");
    
    // Free NULL pointer (should be safe)
    my_free_metabuddy(NULL);
    check(1, "free(NULL) doesn't crash");

}

void test_small_allocation_after_free_big_block_metabuddy() {
    DEBUG_PRINTF("\n--- Testing small allocation after freeing a big block ---\n");

    // Allocate a large block
    void* large_block = my_malloc_metabuddy(512);  // 512 bytes
    check(large_block != NULL, "Allocated 512 bytes block");

    // Allocate a small block
    void* small_block = my_malloc_metabuddy(128);
    check(small_block != NULL, "Allocated 128 bytes block");

    // Free the large block
    my_free_metabuddy(large_block);

    // Now allocate another small block
    void* small_block2 = my_malloc_metabuddy(128);

    check(small_block2 != NULL, "Allocated another 128 bytes block");

    // check that the index of the second small block is before the first block
    check(small_block2 < small_block, "Second small block is before first small block");

    // Free the small block
    my_free_metabuddy(small_block2);

    // Free the first small block
    my_free_metabuddy(small_block);

}

void test_full_allocation_of_buddy_pool_504_metabuddy() {
    DEBUG_PRINTF("\n--- Testing full allocation of buddy pool ---\n");

    // Allocate memory until the 1048576 bytes pool is full with 2048 blocks of 504 bytes
    // 512 bytes = 504 bytes required + 8 bytes of metadata

    void* blocks[2048];
    for (int i = 0; i < 2048; i++) {
        blocks[i] = my_malloc_metabuddy(504);
    }

    // Try to allocate one more block (should fail)
    void* extra = my_malloc_metabuddy(128);
    check(extra == NULL, "Extra allocation fails when pool is full");

    // Free all blocks
    for (int i = 0; i < 2048; i++) {
        my_free_metabuddy(blocks[i]);
    }

}

void test_full_allocation_of_buddy_pool_1015_metabuddy() {
    DEBUG_PRINTF("\n--- Testing full allocation of buddy pool ---\n");

    // Allocate memory until the 1048576 bytes pool is full with 1024 blocks of 1015 bytes
    // 1023 bytes = 1015 bytes required + 8 bytes of metadata
    // It is 1023 instead of 1024 to avoid the threshold check

    void* blocks[1024];
    for (int i = 0; i < 1024; i++) {
        blocks[i] = my_malloc_metabuddy(1015);
    }

    // Try to allocate one more block (should fail)
    void* extra = my_malloc_metabuddy(128);
    check(extra == NULL, "Extra allocation fails when pool is full");

    // Free all blocks
    for (int i = 0; i < 1024; i++) {
        my_free_metabuddy(blocks[i]);
    }
    
}

int main() {

    DEBUG_PRINTF("Running pseudo malloc tests...\n");
    
    int runs = 1000;
    struct timeval t0, t1;
    long long sum_standard = 0;
    long long sum_metabuddy = 0;

    for (int i = 0; i < runs; i++) {
        gettimeofday(&t0, NULL);
        
        test_basic_malloc_free();
        test_different_sizes();
        test_multiple_allocations();
        test_edge_cases();
        test_small_allocation_after_free_big_block();
        test_full_allocation_of_buddy_pool_512();
        test_full_allocation_of_buddy_pool_1023();
        
        gettimeofday(&t1, NULL);

        long long diff = (long long)(t1.tv_sec - t0.tv_sec) * 1000000LL + (long long)(t1.tv_usec - t0.tv_usec);
        sum_standard += diff;
    }

    for (int i = 0; i < runs; i++) {
        gettimeofday(&t0, NULL);

        test_basic_malloc_free_metabuddy();
        test_different_sizes_metabuddy();
        test_multiple_allocations_metabuddy();
        test_edge_cases_metabuddy();
        test_small_allocation_after_free_big_block_metabuddy();
        test_full_allocation_of_buddy_pool_504_metabuddy();
        test_full_allocation_of_buddy_pool_1015_metabuddy();

        gettimeofday(&t1, NULL);

        long long diff = (long long)(t1.tv_sec - t0.tv_sec) * 1000000LL + (long long)(t1.tv_usec - t0.tv_usec);
        sum_metabuddy += diff;
    }

    long long avg_standard = sum_standard / runs;
    long long avg_metabuddy = sum_metabuddy / runs;
    
    printf("Benchmark runs: %d\n", runs);
    printf("Standard buddy allocator: avg %lld us\n", avg_standard);
    printf("Metabuddy allocator:      avg %lld us\n", avg_metabuddy);

    DEBUG_PRINTF("\nResults: %d passed, %d failed\n", passed, failed);
    
    if (failed == 0) {
        DEBUG_PRINTF("All tests passed! 🎉\n");
        return 0;
    } else {
        DEBUG_PRINTF("Some tests failed 😞\n");
        return 1;
    }

}
