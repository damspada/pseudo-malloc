# Makefile for pseudo-malloc project
CC = gcc
CFLAGS = -Wall -std=c99 -g
CFLAGS_DEBUG = $(CFLAGS) -DDEBUG_PRINT
# -Wall : Enable all compiler's warning messages
# -std=c99 : Use C99 standard
# -g : Generate debug information
# -DDEBUG_PRINT : Enable debug printing (when specified)

# Directories
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/bitmap.c $(SRC_DIR)/buddy_allocator.c $(SRC_DIR)/my_malloc.c
OBJECTS = $(BUILD_DIR)/bitmap.o $(BUILD_DIR)/buddy_allocator.o $(BUILD_DIR)/my_malloc.o
OBJECTS_DEBUG = $(BUILD_DIR)/bitmap_debug.o $(BUILD_DIR)/buddy_allocator_debug.o $(BUILD_DIR)/my_malloc_debug.o

# Test files
TESTS = $(TEST_DIR)/test_bitmap $(TEST_DIR)/test_buddy_allocator $(TEST_DIR)/test_my_malloc $(TEST_DIR)/run_tests
TESTS_DEBUG = $(TEST_DIR)/test_bitmap_debug $(TEST_DIR)/test_buddy_allocator_debug $(TEST_DIR)/test_my_malloc_debug $(TEST_DIR)/run_tests_debug

# Default target when i run make without any arguments
all: lib tests

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I include -c $< -o $@

# Compile objects with debug printing
$(BUILD_DIR)/%_debug.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS_DEBUG) -I include -c $< -o $@

# Build library
lib: $(OBJECTS)
	ar rcs $(BUILD_DIR)/libpseudo_malloc.a $(OBJECTS)

# Build library with debug printing
lib_debug: $(OBJECTS_DEBUG)
	ar rcs $(BUILD_DIR)/libpseudo_malloc_debug.a $(OBJECTS_DEBUG)

# Build tests
$(TEST_DIR)/test_bitmap: $(TEST_DIR)/test_bitmap.c $(BUILD_DIR)/bitmap.o
	$(CC) $(CFLAGS) -I include $^ -o $@

$(TEST_DIR)/test_buddy_allocator: $(TEST_DIR)/test_buddy_allocator.c $(BUILD_DIR)/buddy_allocator.o $(BUILD_DIR)/bitmap.o
	$(CC) $(CFLAGS) -I include $^ -o $@

$(TEST_DIR)/test_my_malloc: $(TEST_DIR)/test_my_malloc.c $(OBJECTS)
	$(CC) $(CFLAGS) -I include $^ -o $@

$(TEST_DIR)/run_tests: $(TEST_DIR)/run_tests.c
	$(CC) $(CFLAGS) -I include $< -o $@

# Build tests with debug printing
$(TEST_DIR)/test_bitmap_debug: $(TEST_DIR)/test_bitmap.c $(BUILD_DIR)/bitmap_debug.o
	$(CC) $(CFLAGS_DEBUG) -I include $^ -o $@

$(TEST_DIR)/test_buddy_allocator_debug: $(TEST_DIR)/test_buddy_allocator.c $(BUILD_DIR)/buddy_allocator_debug.o $(BUILD_DIR)/bitmap_debug.o
	$(CC) $(CFLAGS_DEBUG) -I include $^ -o $@

$(TEST_DIR)/test_my_malloc_debug: $(TEST_DIR)/test_my_malloc.c $(OBJECTS_DEBUG)
	$(CC) $(CFLAGS_DEBUG) -I include $^ -o $@

$(TEST_DIR)/run_tests_debug: $(TEST_DIR)/run_tests.c
	$(CC) $(CFLAGS_DEBUG) -I include $< -o $@

tests: $(TESTS)
tests_debug: $(TESTS_DEBUG)

# Run tests (with debug output)
test: tests_debug
	@./$(TEST_DIR)/run_tests_debug

# Run tests (without debug output - silent/performance mode)
test_noprint: tests
	@./$(TEST_DIR)/run_tests

# Clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TESTS) $(TESTS_DEBUG)

.PHONY: all lib lib_debug tests tests_debug test test_noprint clean
