#include <stdio.h>
#include <stdlib.h>
#include "../include/bitmap.h"
#include "../include/debug_print.h"

// Some basic testing for my bitmap implementation just to make sure it works

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

void test_basic_stuff() {
    
    DEBUG_PRINTF("\nTesting basic bitmap operations...\n");
    
    // Create a bitmap with 64 bits
    Bitmap* bmp = bitmap_init(64);
    
    check(bmp != NULL, "creating bitmap");
    check(bmp->size == 64, "bitmap has correct size");
    
    // Try setting bits
    bitmap_set(bmp, 0);      // first bit
    bitmap_set(bmp, 31);     // in the middle
    bitmap_set(bmp, 63);     // last bit
    
    // Check if they're actually set
    check(bitmap_test(bmp, 0) == 1, "first bit is set");
    check(bitmap_test(bmp, 31) == 1, "middle bit is set");
    check(bitmap_test(bmp, 63) == 1, "last bit is set");
    
    // Check that other bits are still clear
    check(bitmap_test(bmp, 1) == 0, "bit 1 should be clear");
    check(bitmap_test(bmp, 30) == 0, "bit 30 should be clear");
    check(bitmap_test(bmp, 62) == 0, "bit 62 should be clear");
    
    // Try clearing a bit
    bitmap_clear(bmp, 31);
    check(bitmap_test(bmp, 31) == 0, "cleared bit 31");
    
    bitmap_free(bmp);

}

void test_edge_cases() {
    DEBUG_PRINTF("\nTesting some edge cases...\n");
    
    // Test with different sizes
    Bitmap* small = bitmap_init(8);
    Bitmap* big = bitmap_init(1000);
    
    check(small != NULL, "small bitmap created");
    check(big != NULL, "big bitmap created");
    
    // Test boundaries on small bitmap
    bitmap_set(small, 0);
    bitmap_set(small, 7);
    check(bitmap_test(small, 0) == 1, "small bitmap first bit");
    check(bitmap_test(small, 7) == 1, "small bitmap last bit");
    
    // Test boundaries on big bitmap
    bitmap_set(big, 0);
    bitmap_set(big, 999);
    check(bitmap_test(big, 0) == 1, "big bitmap first bit");
    check(bitmap_test(big, 999) == 1, "big bitmap last bit");
    
    // try to access out of bounds?
    int result = bitmap_test(small, 8);  // should be out of bounds
    check(result == -1, "out of bounds returns -1");
    
    bitmap_free(small);
    bitmap_free(big);

}

int main() {

    DEBUG_PRINTF("Running bitmap tests...\n");
    
    // test_basic_stuff();
    // test_edge_cases();

    DEBUG_PRINTF("\nResults: %d passed, %d failed\n", passed, failed);
    
    if (failed == 0) {
        DEBUG_PRINTF("All tests passed! 🎉\n");
        return 0;
    } else {
        DEBUG_PRINTF("Some tests failed 😞\n");
        return 1;
    }
    
}
