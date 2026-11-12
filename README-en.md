# CGTL (C Generic Template Library)

[中文](README.md) | [English](README-en.md)

CGTL is a C container and algorithm library inspired by the C++ STL.

The current implementation includes:

- Sequence containers
- Ordered associative containers
- Unordered associative containers
- Container adaptors
- String containers
- Unified iterator and range abstractions
- Generic algorithms instantiated through macro templates

---

## Directory structure

```text
include/
├── cgtl.h                  # Common macros and helper definitions
Sequence Containers
├── array.h                 # Fixed-size array
├── vector.h                # Dynamic array
├── forward_list.h          # Singly linked list
├── list.h                  # Doubly linked list
├── deque.h                 # Double-ended queue
Associative Containers
├── set.h                   # Set
├── multiset.h              # Multiset
├── map.h                   # Map
├── multimap.h              # Multimap
Unordered Associative Containers
├── unordered_set.h         # Unordered set
├── unordered_multiset.h    # Unordered multiset
├── unordered_map.h         # Unordered map
├── unordered_multimap.h    # Unordered multimap
Container Adaptors
├── stack.h                 # Stack
├── queue.h                 # Queue
├── pri_queue.h             # Priority queue
Algorithms and Iterators
├── iterator.h              # Unified iterator / range / iterator category
├── algorithm.h             # Generic algorithms instantiated for T
Auxiliary Containers
├── str.h                   # String backed by vector<char>
├── cstring.h               # Standalone dynamic string
└── README.md
```

---

## Quick start

### Using a container

```c
#define T int
#include "vector.h"

int main(void)
{
    vec_int v = vec_int_init();

    vec_int_push_back(&v, 3);
    vec_int_push_back(&v, 1);
    vec_int_push_back(&v, 2);

    printf("front = %d\n", *vec_int_front(&v));
    printf("size  = %zu\n", v.size);

    vec_int_free(&v);
    return 0;
}
```

### Using multiple containers

Container macros use `T` as the element type. Redefine `T` when switching containers:

```c
#define T int
#include "vector.h"
#undef T

#define T int
#include "deque.h"
#undef T
```

Do not instantiate the same container type more than once in the same translation unit.

---

## Unified iterators

`iterator.h` provides:

- `cgtl_iter_category`
- `cgtl_iter`
- `cgtl_iter_ops`
- `cgtl_range`
- `cgtl_out_iter`
- Helpers for advancing, comparing, measuring distance, and positioning iterators

All traversable containers expose a unified interface:

```c
cgtl_range vec_int_iter_range(vec_int *self);
cgtl_range lst_int_iter_range(lst_int *self);
cgtl_range set_int_iter_range(set_int *self);
cgtl_range map_int_int_iter_range(map_int_int *self);
cgtl_range ust_int_iter_range(ust_int *self);
```

Basic usage:

```c
cgtl_range range = vec_int_iter_range(&v);

for(cgtl_iter it = range.first;
    !cgtl_iter_equal(&it, &range.last);
    cgtl_iter_step(&it))
{
    int *value = (int *)it.ref;
    printf("%d\n", *value);
}
```

### Iterator capabilities

| Category | Containers |
|---|---|
| Random access | `array`, `vector`, `deque`, `str`, `cstring` |
| Bidirectional | `list`, `set`, `multiset`, `map`, `multimap` |
| Forward | `forward_list`, `unordered_set`, `unordered_map`, `unordered_multiset`, `unordered_multimap` |
| No general traversal iterator | `stack`, `queue`, `pri_queue` |

The unified iterator for `unordered_multimap` currently points `ref` at the key. Traversing complete key/value pairs requires a further adjustment to its node view.

---

## Generic algorithms

`algorithm.h` is a template header instantiated for element type `T` and intentionally does not use a conventional single-include guard.

### Same-type algorithms

```c
#define T int
#define HOLD
#include "vector.h"
#include "algorithm.h"

static int compare_int(int *a, int *b)
{
    return (*a > *b) - (*a < *b);
}

static void print_int(int *value)
{
    printf("%d\n", *value);
}

int main(void)
{
    vec_int v = vec_int_init();
    vec_int_push_back(&v, 3);
    vec_int_push_back(&v, 1);
    vec_int_push_back(&v, 2);

    cgtl_range range = vec_int_iter_range(&v);

    algo_int_sort(range, compare_int);
    algo_int_for_each(range, print_int);

    vec_int_free(&v);
    return 0;
}
```

Currently available algorithms include:

- Traversal and predicates: `for_each`, `all_of`, `any_of`, `none_of`
- Search and counting: `find`, `find_if`, `find_if_not`, `count`, `count_if`
- Comparison: `equal`, `lexicographical_compare`
- Modification: `copy`, `copy_if`, `transform`, `fill`, `replace`, `replace_if`
- Removal and compaction: `remove`, `remove_if`, `unique`
- Extrema: `min_element`, `max_element`
- Reverse and sorting: `reverse`, `sort`, `is_sorted`
- Ordered lookup: `lower_bound`, `upper_bound`, `binary_search`
- Set operations: `set_union`, `set_intersection`, `set_difference`, `set_symmetric_difference`, `includes`
- Numeric operations: `accumulate` (implemented with a binary callback in C)
- Output-iterator variants: `copy_to`, `transform_to`

### Cross-type algorithms

Cross-type algorithms use the optional `U` macro:

```c
#define T int
#define U double
#include "algorithm.h"
```

This generates:

```text
algo_int_to_double_copy
algo_int_to_double_transform
```

### Algorithm prefixes

Public algorithms generate `algo_<T>_*` names:

```text
algo_int_find
algo_int_sort
algo_int_binary_search
```

Containers generate their own internal prefixes so multiple containers using the same `T` do not produce duplicate definitions:

```text
vec_int_algo_find
lst_int_algo_find
set_int_algo_equal
map_int_int_algo_equal
```

---

## Compatibility wrappers

The original member-style algorithms on containers remain available, but are now implemented by delegating to the algorithm layer.

| Container | Legacy interfaces converted to compatibility wrappers |
|---|---|
| `array` | `ARRAY_equal`, `ARRAY_find`, `ARRAY_count`, `ARRAY_remove_if` |
| `vector` | `VEC_equal`, `VEC_find`, `VEC_sort`, `VEC_ranged_sort`, `VEC_remove_if` |
| `deque` | `DEQ_equal`, `DEQ_find`, `DEQ_sort`, `DEQ_ranged_sort`, `DEQ_remove_if` |
| `list` | `LST_equal`, `LST_find` |
| `forward_list` | `FLST_find` |
| `set` / `multiset` | `SET_equal`, `SET_union`, `SET_intersection`, `SET_difference`, `SET_symmetric_difference` |

The following operations remain implemented inside their containers because they operate on nodes, buckets, or tree structure:

- `list` / `forward_list` `sort`, `merge`, `splice`, `reverse`, `unique`, and `remove_if`
- Specialized lookup interfaces for `map`, `set`, and `unordered_*`
- Bucket, load-factor, and rehash interfaces for `unordered_*`
- Adaptor operations for `stack`, `queue`, and `pri_queue`
- String-specific operations such as `find`, `rfind`, `substr`, and `compare`

### Sorting compatibility notes

Some legacy tests use comparators that return `0/1` booleans, while standard algorithms require a three-way `<0/0/>0` result.

To preserve legacy behavior:

- `VEC_sort`, `VEC_ranged_sort`, `DEQ_sort`, and `DEQ_ranged_sort` use the `legacy_sort` compatibility wrapper
- New code should use `algo_T_sort`
- Comparators in new code should return a negative value, zero, or a positive value

Example:

```c
static int compare_int(int *a, int *b)
{
    return (*a > *b) - (*a < *b);
}
```

---

## Build and test

Build the tests:

```bash
make clean
make -j4 run_tests
```

Run a single test:

```bash
make test_vector
make test_list
make test_deque
make test_set
make test_iterator_algorithm
```

`run_tests` returns a non-zero status if any test fails.

Current test coverage includes:

- `vector`
- `list`
- `deque`
- `queue`
- `stack`
- `pri_queue`
- `set`
- `unordered_set`
- `map`
- `unordered_map`
- `multiset`
- `multimap`
- `unordered_multiset`
- `unordered_multimap`
- `array`
- `forward_list`
- `cstring`
- `iterator/algorithm`

---

## Examples

- `example/vector_autodiff.c`: automatic differentiation example using vector
- `example/algorithm_demo.c`: examples of the new iterator and algorithm interfaces

`example/algorithm_demo.c` demonstrates:

- Obtaining unified ranges from vector and list
- Using `algo_int_for_each`
- Using `algo_int_sort`
- Using `algo_int_find`
- Using `algo_int_count`

---

## Notes

- `algorithm.h` is a `T`-parameterized header and should be included separately for each required element type.
- Public algorithms use `algo_<T>_*`; container-internal algorithms use `algo_<container>_<T>_*`.
- `sort` requires a random-access range; `list` and `forward_list` use their own node-based sorting.
- `unordered` containers do not provide sorting interfaces.
- `stack`, `queue`, and `pri_queue` follow the STL adaptor design and do not expose general traversal iterators.
