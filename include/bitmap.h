#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    size_t size; // Size of the bitmap in bits
    unsigned char* bits; // Pointer to the bitmap bits
} Bitmap;

Bitmap* bitmap_init(size_t size);
void bitmap_free(Bitmap* bitmap);
void bitmap_set(Bitmap* bitmap, size_t index);
void bitmap_clear(Bitmap* bitmap, size_t index);