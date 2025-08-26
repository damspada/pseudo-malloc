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

int main() {

    printf("Running pseudo malloc tests...\n");
    
    test_basic_malloc_free();
    test_different_sizes();
    test_multiple_allocations();
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
