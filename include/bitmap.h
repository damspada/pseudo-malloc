#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    size_t size; // Size of the bitmap in bits
    unsigned char* bits; // Pointer to the bitmap bits
} Bitmap;

// Initialize a new bitmap with the given size
Bitmap* bitmap_init(size_t size);

// Free the memory allocated for a bitmap
void bitmap_free(Bitmap* bitmap);

// Set a bit at the given index (mark as 1)
void bitmap_set(Bitmap* bitmap, size_t index);

// Clear a bit at the given index (mark as 0)
void bitmap_clear(Bitmap* bitmap, size_t index);

// Test if a bit is set (1) or cleared (0)
// Returns -1 on error
int bitmap_test(const Bitmap* bitmap, size_t index);

#endif // BITMAP_H