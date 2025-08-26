#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Simple test runner that just runs all the tests

const char* test_compiled[] = {
    "test/test_bitmap",
    "test/test_buddy_allocator",
    "test/test_my_malloc"
};

const char* test_descriptions[] = {
    "Bitmap functionality",
    "Buddy allocator",
    "Main malloc implementation"
};

int run_single_test(const char* compiled, const char* description) {
    printf("\n=== Running %s ===\n", description);
    
    // Check if the test executable exists
    if (access(compiled, X_OK) != 0) {
        printf("ERROR: Can't find compiled test: %s\n", compiled);
        printf("(forget to compile it?)\n");
        return -1;
    }

    // Check if the test is executable
    if (access(compiled, X_OK) != 0) {
        printf("ERROR: Test is not executable: %s\n", compiled);
        return -1;
    }

    // Run the test
    int result = system(compiled);
    int exit_code = WEXITSTATUS(result);
    
    if (exit_code == 0) {
        printf("✓ %s: PASSED\n", description);
    } else {
        printf("✗ %s: FAILED (exit code: %d)\n", description, exit_code);
    }
    
    return exit_code;
    
}

int main() {
    printf("Running all tests for pseudo-malloc\n");
    printf("===================================\n");

    int total_tests = 3;
    int passed = 0;
    int failed = 0;
    int errors = 0;
    
    for (int i = 0; i < total_tests; i++) {
        int result = run_single_test(test_compiled[i], test_descriptions[i]);
        
        if (result == 0) {
            passed++;
        } else if (result > 0) {
            failed++;
        } else {
            errors++;
        }
    }
    
    printf("\n=== Results ===\n");
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    if (errors > 0) {
        printf("Errors: %d missing test executables\n", errors);
        return 2;
    }
    
    if (passed == total_tests) {
        printf("\nAwesome! Everything works! 🎉\n");
        return 0;
    } else {
        printf("\nSome tests failed 😢\n");
        return 1;
    }
    
}
