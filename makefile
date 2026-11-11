# CGTL - C implementation of C++ STL
# Makefile for building and testing the library

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2 -fPIC
INCLUDES = -I./include -I./lib/cmocka/include
LDFLAGS = -L./lib/cmocka/lib
LDLIBS = -lcmocka

# Directories
SRCDIR = .
TESTDIR = ./test
INCLUDEDIR = ./include
LIBDIR = ./lib
BINDIR = ./bin

# Test settings  
TEST_CFLAGS = $(CFLAGS) $(INCLUDES)
TEST_LDFLAGS = $(LDFLAGS) $(LDLIBS)
# Set LD_LIBRARY_PATH to find cmocka library
export LD_LIBRARY_PATH := $(shell pwd)/lib/cmocka/lib:$(LD_LIBRARY_PATH)

# Create directories
$(BINDIR):
	mkdir -p $(BINDIR)

# Test targets
.PHONY: all test clean help
.PHONY: test_vector test_list test_deque test_queue test_stack test_pri_queue test_set test_unorder_set
.PHONY: test_map test_unordered_map test_multiset test_multimap test_unordered_multiset test_unordered_multimap test_array test_forward_list test_cstring
.PHONY: run_tests run_valgrind

all: test

help:
	@echo "CGTL Makefile Usage:"
	@echo "  all          - Build all tests"
	@echo "  test         - Build and run all tests"
	@echo "  run_tests    - Run all tests without valgrind"
	@echo "  run_valgrind - Run all tests with valgrind memory check"
	@echo "  test_<name>  - Build and run specific test (vector, list, deque, etc.)"
	@echo "  Available containers: vector, list, deque, queue, stack, pri_queue, set, unorder_set"
	@echo "                       map, unordered_map, multiset, multimap, unordered_multiset"
	@echo "                       unordered_multimap, array, forward_list, cstring"
	@echo "  clean        - Remove all generated files"
	@echo "  help         - Show this help message"

# Main test target
test: $(BINDIR) test_vector test_list test_deque test_queue test_stack test_pri_queue test_set test_unorder_set test_map test_unordered_map test_multiset test_multimap test_unordered_multiset test_unordered_multimap test_array test_forward_list test_cstring

# Run tests without valgrind (build all tests first)
run_tests: $(BINDIR)/test_vector $(BINDIR)/test_list $(BINDIR)/test_deque $(BINDIR)/test_queue $(BINDIR)/test_stack $(BINDIR)/test_pri_queue $(BINDIR)/test_set $(BINDIR)/test_unorder_set $(BINDIR)/test_map $(BINDIR)/test_unordered_map $(BINDIR)/test_multiset $(BINDIR)/test_multimap $(BINDIR)/test_unordered_multiset $(BINDIR)/test_unordered_multimap $(BINDIR)/test_array $(BINDIR)/test_forward_list $(BINDIR)/test_cstring
	@echo "Running all tests without memory checking..."
	@for test in vector list deque queue stack pri_queue set unorder_set map unordered_map multiset multimap unordered_multiset unordered_multimap array forward_list cstring; do \
		echo "=== Running test_$$test ==="; \
		LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH $(BINDIR)/test_$$test || echo "FAILED: test_$$test"; \
		echo; \
	done

# Run tests with valgrind (build all tests first)  
run_valgrind: $(BINDIR)/test_vector $(BINDIR)/test_list $(BINDIR)/test_deque $(BINDIR)/test_queue $(BINDIR)/test_stack $(BINDIR)/test_pri_queue $(BINDIR)/test_set $(BINDIR)/test_unorder_set $(BINDIR)/test_map $(BINDIR)/test_unordered_map $(BINDIR)/test_multiset $(BINDIR)/test_multimap $(BINDIR)/test_unordered_multiset $(BINDIR)/test_unordered_multimap $(BINDIR)/test_array $(BINDIR)/test_forward_list $(BINDIR)/test_cstring
	@echo "Running all tests with valgrind memory checking..."
	@for test in vector list deque queue stack pri_queue set unorder_set map unordered_map multiset multimap unordered_multiset unordered_multimap array forward_list cstring; do \
		echo "=== Running test_$$test with valgrind ==="; \
		LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
		valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
			--num-callers=20 --track-fds=yes --track-origins=yes \
			$(BINDIR)/test_$$test || echo "FAILED: test_$$test"; \
		echo; \
	done

# Individual test targets
test_vector: $(BINDIR)/test_vector
	@echo "=== Testing Vector ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_vector

test_list: $(BINDIR)/test_list
	@echo "=== Testing List ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_list

test_deque: $(BINDIR)/test_deque
	@echo "=== Testing Deque ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_deque

test_queue: $(BINDIR)/test_queue
	@echo "=== Testing Queue ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_queue

test_stack: $(BINDIR)/test_stack
	@echo "=== Testing Stack ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_stack

test_pri_queue: $(BINDIR)/test_pri_queue
	@echo "=== Testing Priority Queue ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_pri_queue

test_set: $(BINDIR)/test_set
	@echo "=== Testing Set ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_set

test_unorder_set: $(BINDIR)/test_unorder_set
	@echo "=== Testing Unordered Set ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_unorder_set

test_map: $(BINDIR)/test_map
	@echo "=== Testing Map ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_map

test_unordered_map: $(BINDIR)/test_unordered_map
	@echo "=== Testing Unordered Map ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_unordered_map

test_multiset: $(BINDIR)/test_multiset
	@echo "=== Testing Multiset ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_multiset

test_multimap: $(BINDIR)/test_multimap
	@echo "=== Testing Multimap ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_multimap

test_unordered_multiset: $(BINDIR)/test_unordered_multiset
	@echo "=== Testing Unordered Multiset ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_unordered_multiset

test_unordered_multimap: $(BINDIR)/test_unordered_multimap
	@echo "=== Testing Unordered Multimap ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_unordered_multimap

test_array: $(BINDIR)/test_array
	@echo "=== Testing Array ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_array

test_forward_list: $(BINDIR)/test_forward_list
	@echo "=== Testing Forward List ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_forward_list

test_cstring: $(BINDIR)/test_cstring
	@echo "=== Testing String ==="
	LD_LIBRARY_PATH=./lib/cmocka/lib:$$LD_LIBRARY_PATH \
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes --track-origins=yes \
		$(BINDIR)/test_cstring

# Test binary compilation rules
$(BINDIR)/test_vector: $(TESTDIR)/test_vector.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS) -lm

$(BINDIR)/test_list: $(TESTDIR)/test_list.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_deque: $(TESTDIR)/test_deque.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS) -lm

$(BINDIR)/test_queue: $(TESTDIR)/test_queue.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_stack: $(TESTDIR)/test_stack.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_pri_queue: $(TESTDIR)/test_pri_queue.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_set: $(TESTDIR)/test_set.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_unorder_set: $(TESTDIR)/test_unorder_set.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_map: $(TESTDIR)/test_map.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_unordered_map: $(TESTDIR)/test_unordered_map.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_multiset: $(TESTDIR)/test_multiset.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_multimap: $(TESTDIR)/test_multimap.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_unordered_multiset: $(TESTDIR)/test_unordered_multiset.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_unordered_multimap: $(TESTDIR)/test_unordered_multimap.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_array: $(TESTDIR)/test_array.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_forward_list: $(TESTDIR)/test_forward_list.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

$(BINDIR)/test_cstring: $(TESTDIR)/test_cstring.c $(TESTDIR)/test_helpers.c | $(BINDIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)

# Clean target
clean:
	rm -rf $(BINDIR)
	rm -f $(TESTDIR)/*.bin
	rm -f core vgcore.*
	@echo "Cleaned all generated files"

# Debug target to show variables
debug:
	@echo "CC = $(CC)"
	@echo "CFLAGS = $(CFLAGS)"
	@echo "INCLUDES = $(INCLUDES)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "LDLIBS = $(LDLIBS)"
	@echo "TESTDIR = $(TESTDIR)"
	@echo "BINDIR = $(BINDIR)"
