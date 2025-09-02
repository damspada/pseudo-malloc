# Makefile for pseudo-malloc project
CC = gcc
CFLAGS = -Wall -std=c99
# -Wall : Enable all compiler's warning messages
# -std=c99 : Use C99 standard

# IF YOU WANT TO SEE PRINTF AND DEBUG
# -g : Generate debug information
# -DDEBUG_PRINT : Enable debug printing (when specified)

# Directories
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/bitmap.c $(SRC_DIR)/buddy_allocator.c $(SRC_DIR)/my_malloc.c
OBJECTS = $(BUILD_DIR)/bitmap.o $(BUILD_DIR)/buddy_allocator.o $(BUILD_DIR)/my_malloc.o

# Test files
TESTS = $(TEST_DIR)/test_bitmap $(TEST_DIR)/test_buddy_allocator $(TEST_DIR)/test_my_malloc $(TEST_DIR)/run_tests

# Default target when i run make without any arguments
all: lib tests

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build library
lib: $(OBJECTS)
	ar rcs $(BUILD_DIR)/libpseudo_malloc.a $(OBJECTS)

# Build tests
$(TEST_DIR)/test_bitmap: $(TEST_DIR)/test_bitmap.c $(BUILD_DIR)/bitmap.o
	$(CC) $(CFLAGS) -I include $^ -o $@

$(TEST_DIR)/test_buddy_allocator: $(TEST_DIR)/test_buddy_allocator.c $(BUILD_DIR)/buddy_allocator.o $(BUILD_DIR)/bitmap.o
	$(CC) $(CFLAGS) -I include $^ -o $@

$(TEST_DIR)/test_my_malloc: $(TEST_DIR)/test_my_malloc.c $(OBJECTS)
	$(CC) $(CFLAGS) -I include $^ -o $@

$(TEST_DIR)/run_tests: $(TEST_DIR)/run_tests.c
	$(CC) $(CFLAGS) -I include $< -o $@

tests: $(TESTS)

# Run tests
test: tests
	@./$(TEST_DIR)/run_tests

# Clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TESTS)

.PHONY: all lib tests test clean
