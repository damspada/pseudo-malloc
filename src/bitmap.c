#include "../include/bitmap.h"

Bitmap* bitmap_init(size_t size) {
    Bitmap* bitmap = (Bitmap*)malloc(sizeof(Bitmap));
    
    if (!bitmap) {
        fprintf(stderr, "[bitmap_init]: Allocation of bitmap struct failed\n");
        return NULL;
    }

    bitmap->size = size;
    bitmap->bits = (unsigned char*)malloc((size + 7) / 8 * sizeof(unsigned char));
    // We round up to the nearest byte by adding 7 before dividing by 8.
    // This ensures that we have enough space for all bits.
    
    if (!bitmap->bits) {
        fprintf(stderr, "[bitmap_init]: Allocation of bitmap bits failed\n");
        free(bitmap);
        return NULL;
    }
    
    // Initialize all bits to 0
    memset(bitmap->bits, 0, (size + 7) / 8 * sizeof(unsigned char));

    return bitmap;
}

void bitmap_free(Bitmap* bitmap){
    
    if (!bitmap) {
        fprintf(stderr, "[bitmap_free]: Invalid bitmap pointer\n");
        return;
    }
    
    free(bitmap->bits);
    free(bitmap);

}

void bitmap_set(Bitmap* bitmap, size_t index) {
    
    if (!bitmap) {
        fprintf(stderr, "[bitmap_set]: Invalid bitmap pointer\n");
        return;
    }

    if (index >= bitmap->size) {
        fprintf(stderr, "[bitmap_set]: Index out of bounds\n");
        return;
    }

    // For setting a bit:
    // 1. Identify the byte: index / 8
    // 2. Identify the bit within the byte: index % 8
    // 3. Use bitwise OR to set the bit
    bitmap->bits[index / 8] |= (1 << (index % 8));
    
}

void bitmap_clear(Bitmap* bitmap, size_t index) {

    if (!bitmap) {
        fprintf(stderr, "[bitmap_clear]: Invalid bitmap pointer\n");
        return;
    }

    if (index >= bitmap->size) {
        fprintf(stderr, "[bitmap_clear]: Index out of bounds\n");
        return;
    }

    // For clearing a bit:
    // 1. Identify the byte: index / 8
    // 2. Identify the bit within the byte: index % 8
    // 3. Use bitwise AND with the negation to clear the bit
    bitmap->bits[index / 8] &= ~(1 << (index % 8));

}
