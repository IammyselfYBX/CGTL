#ifndef T
#error "Template type T undefined for <multiset.h>"
#endif

#include "cgtl.h"

#define MULTISET JOIN(multiset, T)
#define MULTISETNODE JOIN(MULTISET, node)
#define MULTISETIT JOIN(MULTISET, it)

#define ALLOW_DUPLICATES
#define HOLD
#include "set.h"
#undef HOLD

typedef JOIN(set, T) MULTISET;
typedef JOIN(JOIN(set, T), node) MULTISETNODE;
typedef JOIN(JOIN(set, T), it) MULTISETIT;

#define CGTL_ALGO_PREFIX JOIN(MULTISET, algo)
#include "algorithm.h"
#undef CGTL_ALGO_PREFIX

static inline cgtl_iter
JOIN(MULTISET, iter_begin)(MULTISET* self)
{
    return JOIN(JOIN(set, T), iter_begin)(self);
}

static inline cgtl_iter
JOIN(MULTISET, iter_end)(MULTISET* self)
{
    return JOIN(JOIN(set, T), iter_end)(self);
}

static inline cgtl_range
JOIN(MULTISET, iter_range)(MULTISET* self)
{
    return JOIN(JOIN(set, T), iter_range)(self);
}

static inline MULTISET
JOIN(MULTISET, init)(int _compare(T*, T*))
{
    return JOIN(JOIN(set, T), init)(_compare);
}

static inline MULTISETNODE*
JOIN(MULTISET, insert)(MULTISET* self, T key)
{
    return JOIN(JOIN(set, T), insert)(self, key);
}

static inline MULTISETNODE*
JOIN(MULTISET, find)(MULTISET* self, T key)
{
    return JOIN(JOIN(set, T), find)(self, key);
}

static inline void
JOIN(MULTISET, erase)(MULTISET* self, T key)
{
    JOIN(JOIN(set, T), erase)(self, key);
}

static inline size_t
JOIN(MULTISET, erase_all)(MULTISET* self, T key)
{
    return JOIN(JOIN(set, T), erase_all)(self, key);
}

static inline size_t
JOIN(MULTISET, count)(MULTISET* self, T key)
{
    return JOIN(JOIN(set, T), count_all)(self, key);
}

static inline void
JOIN(MULTISET, equal_range)(MULTISET* self, T key, MULTISETNODE** begin, MULTISETNODE** end)
{
    JOIN(JOIN(set, T), equal_range)(self, key, begin, end);
}

static inline MULTISETNODE*
JOIN(MULTISET, lower_bound)(MULTISET* self, T key)
{
    return JOIN(JOIN(set, T), lower_bound)(self, key);
}

static inline MULTISETNODE*
JOIN(MULTISET, upper_bound)(MULTISET* self, T key)
{
    return JOIN(JOIN(set, T), upper_bound)(self, key);
}

static inline void
JOIN(MULTISET, clear)(MULTISET* self)
{
    JOIN(JOIN(set, T), clear)(self);
}

static inline void
JOIN(MULTISET, free)(MULTISET* self)
{
    JOIN(JOIN(set, T), free)(self);
}

static inline MULTISET
JOIN(MULTISET, shallow_copy)(MULTISET* self)
{
    return JOIN(JOIN(set, T), shallow_copy)(self);
}

static inline MULTISET
JOIN(MULTISET, deep_copy)(MULTISET* self)
{
    return JOIN(JOIN(set, T), deep_copy)(self);
}

static inline void
JOIN(MULTISET, swap)(MULTISET* self, MULTISET* other)
{
    JOIN(JOIN(set, T), swap)(self, other);
}

static inline int
JOIN(MULTISET, empty)(MULTISET* self)
{
    return JOIN(JOIN(set, T), empty)(self);
}

static inline size_t
JOIN(MULTISET, size)(MULTISET* self)
{
    return self->size;
}

static inline size_t
JOIN(MULTISET, remove_if)(MULTISET* self, int _match(T*))
{
    return JOIN(JOIN(set, T), remove_if)(self, _match);
}

static inline int
JOIN(MULTISET, equal)(MULTISET* self, MULTISET* other, int _equal(T*, T*))
{
    return JOIN(JOIN(set, T), equal)(self, other, _equal);
}

static inline MULTISET
JOIN(MULTISET, intersection)(MULTISET* a, MULTISET* b)
{
    return JOIN(JOIN(set, T), intersection)(a, b);
}

static inline MULTISET
JOIN(MULTISET, union)(MULTISET* a, MULTISET* b)
{
    return JOIN(JOIN(set, T), union)(a, b);
}

static inline MULTISET
JOIN(MULTISET, difference)(MULTISET* a, MULTISET* b)
{
    return JOIN(JOIN(set, T), difference)(a, b);
}

static inline MULTISET
JOIN(MULTISET, symmetric_difference)(MULTISET* a, MULTISET* b)
{
    return JOIN(JOIN(set, T), symmetric_difference)(a, b);
}

static inline MULTISETIT
JOIN(MULTISETIT, each)(MULTISET* self)
{
    return JOIN(JOIN(JOIN(set, T), it), each)(self);
}

static inline MULTISETIT
JOIN(MULTISETIT, range)(MULTISET* self, MULTISETNODE* begin, MULTISETNODE* end)
{
    return JOIN(JOIN(JOIN(set, T), it), range)(self, begin, end);
}

static inline MULTISETNODE*
JOIN(MULTISET, begin)(MULTISET* self)
{
    return JOIN(JOIN(set, T), begin)(self);
}

static inline MULTISETNODE*
JOIN(MULTISET, end)(MULTISET* self)
{
    return JOIN(JOIN(set, T), end)(self);
}

#undef ALLOW_DUPLICATES
#undef T
#undef MULTISET
#undef MULTISETNODE
#undef MULTISETIT
