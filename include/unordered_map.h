#ifndef K
#error "Template key type K undefined for <unordered_map.h>"
#endif

#ifndef V
#error "Template value type V undefined for <unordered_map.h>"
#endif

#include "cgtl.h"
#include "iterator.h"

#define UPAIR JOIN(JOIN(upair, K), V)
#define UMAP JOIN(JOIN(umap, K), V)
#define UMAPNODE JOIN(UMAP, node)
#define UMAPIT JOIN(UMAP, it)

typedef struct UPAIR
{
    K first;
    V second;
} UPAIR;

static inline size_t
JOIN(UPAIR, hash_key)(UPAIR* self)
{

    extern size_t JOIN(K, hash)(K*);
    return JOIN(K, hash)(&self->first);
}

static inline int
JOIN(UPAIR, equal_key)(UPAIR* a, UPAIR* b)
{

    extern int JOIN(K, equal)(K*, K*);
    return JOIN(K, equal)(&a->first, &b->first);
}

static inline UPAIR
JOIN(UPAIR, shallow_copy)(UPAIR* self)
{
    UPAIR result;
    result.first = self->first;
    result.second = self->second;
    return result;
}

static inline UPAIR
JOIN(UPAIR, deep_copy)(UPAIR* self)
{
    UPAIR result;

    result.first = self->first;
    result.second = self->second;
    return result;
}

static inline void
JOIN(UPAIR, deep_free)(UPAIR* self)
{

    (void)self;
}

#define T UPAIR
#define HOLD
#include "unordered_set.h"
#undef T

typedef JOIN(ust, UPAIR) UMAP;
typedef JOIN(JOIN(ust, UPAIR), node) UMAPNODE;
typedef JOIN(JOIN(ust, UPAIR), it) UMAPIT;

#define T UPAIR
#define CGTL_ALGO_PREFIX JOIN(UMAP, algo)
#include "algorithm.h"
#undef T
#undef CGTL_ALGO_PREFIX

static inline cgtl_iter
JOIN(UMAP, iter_begin)(UMAP* self)
{
    return JOIN(JOIN(ust, UPAIR), iter_begin)(self);
}

static inline cgtl_iter
JOIN(UMAP, iter_end)(UMAP* self)
{
    return JOIN(JOIN(ust, UPAIR), iter_end)(self);
}

static inline cgtl_range
JOIN(UMAP, iter_range)(UMAP* self)
{
    return JOIN(JOIN(ust, UPAIR), iter_range)(self);
}

static inline UMAP
JOIN(UMAP, init)(size_t _hash(K*), int _equal(K*, K*))
{
    return JOIN(JOIN(ust, UPAIR), init)(JOIN(UPAIR, hash_key), JOIN(UPAIR, equal_key));
}

static inline void
JOIN(UMAP, insert)(UMAP* self, K key, V value)
{
    UPAIR pair;
    pair.first = key;
    pair.second = value;

    UMAPNODE* existing = JOIN(JOIN(ust, UPAIR), find)(self, pair);
    if (existing)
    {

        existing->key.second = value;
    }
    else
    {

        JOIN(JOIN(ust, UPAIR), insert)(self, pair);
    }
}

static inline UMAPNODE*
JOIN(UMAP, find)(UMAP* self, K key)
{
    UPAIR temp;
    temp.first = key;

    return JOIN(JOIN(ust, UPAIR), find)(self, temp);
}

static inline V*
JOIN(UMAP, at)(UMAP* self, K key)
{
    UMAPNODE* node = JOIN(UMAP, find)(self, key);
    return node ? &node->key.second : NULL;
}

static inline void
JOIN(UMAP, erase)(UMAP* self, K key)
{
    UPAIR temp;
    temp.first = key;
    JOIN(JOIN(ust, UPAIR), erase)(self, temp);
}

static inline size_t
JOIN(UMAP, count)(UMAP* self, K key)
{
    UPAIR temp;
    temp.first = key;
    return JOIN(JOIN(ust, UPAIR), count)(self, temp);
}

static inline void
JOIN(UMAP, clear)(UMAP* self)
{
    JOIN(JOIN(ust, UPAIR), clear)(self);
}

static inline void
JOIN(UMAP, free)(UMAP* self)
{
    JOIN(JOIN(ust, UPAIR), free)(self);
}

static inline UMAP
JOIN(UMAP, shallow_copy)(UMAP* self)
{
    return JOIN(JOIN(ust, UPAIR), shallow_copy)(self);
}

static inline UMAP
JOIN(UMAP, deep_copy)(UMAP* self)
{
    return JOIN(JOIN(ust, UPAIR), deep_copy)(self);
}

static inline void
JOIN(UMAP, swap)(UMAP* self, UMAP* other)
{
    JOIN(JOIN(ust, UPAIR), swap)(self, other);
}

static inline int
JOIN(UMAP, empty)(UMAP* self)
{
    return JOIN(JOIN(ust, UPAIR), empty)(self);
}

static inline size_t
JOIN(UMAP, size)(UMAP* self)
{
    return self->size;
}

static inline size_t
JOIN(UMAP, remove_if)(UMAP* self, int _match(K*))
{
    return JOIN(JOIN(ust, UPAIR), remove_if)(self, (int (*)(UPAIR*))_match);
}

static inline int
JOIN(UMAP, equal)(UMAP* self, UMAP* other)
{
    return JOIN(JOIN(ust, UPAIR), equal)(self, other);
}

static inline UMAPIT
JOIN(UMAPIT, each)(UMAP* self)
{
    return JOIN(JOIN(JOIN(ust, UPAIR), it), each)(self);
}

static inline UMAPNODE*
JOIN(UMAP, begin)(UMAP* self)
{
    return JOIN(JOIN(ust, UPAIR), begin)(self);
}

static inline UMAPNODE*
JOIN(UMAP, end)(UMAP* self)
{
    return JOIN(JOIN(ust, UPAIR), end)(self);
}

static inline size_t
JOIN(UMAP, bucket_count)(UMAP* self)
{
    return self->bucket_count;
}

static inline size_t
JOIN(UMAP, bucket_size)(UMAP* self, size_t index)
{
    return JOIN(JOIN(ust, UPAIR), bucket_size)(self, index);
}

static inline float
JOIN(UMAP, load_factor)(UMAP* self)
{
    return JOIN(JOIN(ust, UPAIR), load_factor)(self);
}

static inline void
JOIN(UMAP, reserve)(UMAP* self, size_t count)
{
    JOIN(JOIN(ust, UPAIR), reserve)(self, count);
}

static inline void
JOIN(UMAP, rehash)(UMAP* self, size_t count)
{
    JOIN(JOIN(ust, UPAIR), rehash)(self, count);
}

#undef K
#undef V
#undef UPAIR
#undef UMAP
#undef UMAPNODE
#undef UMAPIT
