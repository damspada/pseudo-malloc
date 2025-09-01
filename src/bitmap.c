#include "../include/bitmap.h"
#include "../include/debug_print.h"

Bitmap* bitmap_init(size_t size) {
    Bitmap* bitmap = (Bitmap*)malloc(sizeof(Bitmap));
    
    if (!bitmap) {
        DEBUG_FPRINTF(stderr, "[bitmap_init]: Allocation of bitmap struct failed\n");
        return NULL;
    }

    bitmap->size = size;

    // We round up to the nearest byte by adding 7 before dividing by 8.
    // This ensures that we have enough space for all bits.
    size_t bytes_needed = (size + 7) / 8;
    bitmap->bits = (unsigned char*)malloc(bytes_needed * sizeof(unsigned char));
    
    if (!bitmap->bits) {
        DEBUG_FPRINTF(stderr, "[bitmap_init]: Error: Allocation of bitmap bits failed\n");
        free(bitmap);
        return NULL;
    }
    
    // Initialize all bits to 0
    if (memset(bitmap->bits, 0, bytes_needed * sizeof(unsigned char)) == NULL) {
        DEBUG_FPRINTF(stderr, "[bitmap_init]: Error: Setting bits to 0 failed\n");
        free(bitmap->bits);
        free(bitmap);
        return NULL;
    }

    return bitmap;
}

void bitmap_free(Bitmap* bitmap){
    
    if (!bitmap) {
        DEBUG_FPRINTF(stderr, "[bitmap_free]: Error: Invalid bitmap pointer\n");
        return;
    }
    
    free(bitmap->bits);
    free(bitmap);
    
}

void bitmap_set(Bitmap* bitmap, size_t index) {
    
    if (!bitmap) {
        DEBUG_FPRINTF(stderr, "[bitmap_set]: Error: Invalid bitmap pointer\n");
        return;
    }

    if (index >= bitmap->size) {
        DEBUG_FPRINTF(stderr, "[bitmap_set]: Error: Index %zu out of bounds (max: %zu)\n", 
                index, bitmap->size - 1);
        return;
    }

    // For setting a bit:
    // 1. Get the byte containing the bit
    // 2. Create a mask for the specific bit
    // 3. Use bitwise OR to set it
    int byte_index = index / 8; // Get the byte index
    int mask = (1 << (index % 8)); // Create a mask for the specific bit
    bitmap->bits[byte_index] |= mask; // Set the bit using OR operation

}

void bitmap_clear(Bitmap* bitmap, size_t index) {

    if (!bitmap) {
        DEBUG_FPRINTF(stderr, "[bitmap_clear]: Error: Invalid bitmap pointer\n");
        return;
    }

    if (index >= bitmap->size) {
        DEBUG_FPRINTF(stderr, "[bitmap_clear]: Error: Index %zu out of bounds (max: %zu)\n", 
                index, bitmap->size - 1);
        return;
    }

    // For clearing a bit:
    // 1. Get the byte containing the bit
    // 2. Create a mask for the specific bit
    // 3. Use bitwise AND with the negation to clear it
    int byte_index = index / 8; // Get the byte index
    int mask = (1 << (index % 8)); // Create a mask for the specific bit
    bitmap->bits[byte_index] &= ~mask; // Clear the bit using negated mask
}

// Test if a bit is set or not
int bitmap_test(const Bitmap* bitmap, size_t index) {
    if (!bitmap) {
        DEBUG_FPRINTF(stderr, "[bitmap_test]: Error: Invalid bitmap pointer\n");
        return -1;
    }

    if (index >= bitmap->size) {
        DEBUG_FPRINTF(stderr, "[bitmap_test]: Error: Index %zu out of bounds (max: %zu)\n", 
                index, bitmap->size - 1);
        return -1;
    }

    int byte_index = bitmap->bits[index / 8]; // Get the byte containing the bit
    int mask = (1 << (index % 8)); // Create a mask for the specific bit
    int bit_value = byte_index & mask; // Mask application
    return bit_value ? 1 : 0; // Return 1 if set, 0 if not

}
