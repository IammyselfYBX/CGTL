#ifndef K
#error "Template key type K undefined for <map.h>"
#endif

#ifndef V
#error "Template value type V undefined for <map.h>"
#endif

#include "cgtl.h"

#define PAIR JOIN(JOIN(pair, K), V)
#define MAP JOIN(JOIN(map, K), V)
#define MAPNODE JOIN(MAP, node)
#define MAPIT JOIN(MAP, it)

typedef struct PAIR
{
    K first;
    V second;
} PAIR;

#define T PAIR
#define HOLD
#include "set.h"
#undef T

typedef JOIN(set, PAIR) MAP;
typedef JOIN(JOIN(set, PAIR), node) MAPNODE;
typedef JOIN(JOIN(set, PAIR), it) MAPIT;

#define T PAIR
#define CGTL_ALGO_PREFIX JOIN(MAP, algo)
#include "algorithm.h"
#undef T
#undef CGTL_ALGO_PREFIX

static inline cgtl_iter
JOIN(MAP, iter_begin)(MAP* self)
{
    return JOIN(JOIN(set, PAIR), iter_begin)(self);
}

static inline cgtl_iter
JOIN(MAP, iter_end)(MAP* self)
{
    return JOIN(JOIN(set, PAIR), iter_end)(self);
}

static inline cgtl_range
JOIN(MAP, iter_range)(MAP* self)
{
    return JOIN(JOIN(set, PAIR), iter_range)(self);
}

static inline int
JOIN(PAIR, compare_key)(PAIR* a, PAIR* b)
{

    extern int JOIN(K, compare)(K*, K*);
    return JOIN(K, compare)(&a->first, &b->first);
}

static inline PAIR
JOIN(PAIR, shallow_copy)(PAIR* self)
{
    PAIR result;
    result.first = self->first;
    result.second = self->second;
    return result;
}

static inline PAIR
JOIN(PAIR, deep_copy)(PAIR* self)
{
    PAIR result;
    extern K JOIN(K, copy)(K*);
    extern V JOIN(V, copy)(V*);
    result.first = JOIN(K, copy)(&self->first);
    result.second = JOIN(V, copy)(&self->second);
    return result;
}

static inline void
JOIN(PAIR, free)(PAIR* self)
{

    (void)self;
}

static inline MAP
JOIN(MAP, init)(int _compare(K*, K*))
{
    (void)_compare;
    return JOIN(JOIN(set, PAIR), init)(JOIN(PAIR, compare_key));
}

static inline MAPNODE*
JOIN(MAP, insert)(MAP* self, K key, V value)
{
    PAIR pair;
    pair.first = key;
    pair.second = value;

    MAPNODE* existing = JOIN(JOIN(set, PAIR), find)(self, pair);
    if (existing)
    {

        if (self->free)
            self->free(&existing->key);
        existing->key = self->copy ? self->copy(&pair) : JOIN(PAIR, shallow_copy)(&pair);
        return existing;
    }
    else
    {

        return JOIN(JOIN(set, PAIR), insert)(self, self->copy ? self->copy(&pair) : JOIN(PAIR, shallow_copy)(&pair));
    }
}

static inline MAPNODE*
JOIN(MAP, find)(MAP* self, K key)
{
    PAIR temp;
    temp.first = key;

    return JOIN(JOIN(set, PAIR), find)(self, temp);
}

static inline V*
JOIN(MAP, at)(MAP* self, K key)
{
    MAPNODE* node = JOIN(MAP, find)(self, key);
    return node ? &node->key.second : NULL;
}

static inline void
JOIN(MAP, erase)(MAP* self, K key)
{
    PAIR temp;
    temp.first = key;
    JOIN(JOIN(set, PAIR), erase)(self, temp);
}

static inline int
JOIN(MAP, count)(MAP* self, K key)
{
    PAIR temp;
    temp.first = key;
    return JOIN(JOIN(set, PAIR), count)(self, temp);
}

static inline MAPNODE*
JOIN(MAP, lower_bound)(MAP* self, K key)
{
    PAIR temp;
    temp.first = key;
    return JOIN(JOIN(set, PAIR), lower_bound)(self, temp);
}

static inline MAPNODE*
JOIN(MAP, upper_bound)(MAP* self, K key)
{
    PAIR temp;
    temp.first = key;
    return JOIN(JOIN(set, PAIR), upper_bound)(self, temp);
}

static inline void
JOIN(MAP, clear)(MAP* self)
{
    JOIN(JOIN(set, PAIR), clear)(self);
}

static inline void
JOIN(MAP, free)(MAP* self)
{
    JOIN(JOIN(set, PAIR), free)(self);
}

static inline MAP
JOIN(MAP, shallow_copy)(MAP* self)
{
    return JOIN(JOIN(set, PAIR), shallow_copy)(self);
}

static inline MAP
JOIN(MAP, deep_copy)(MAP* self)
{
    return JOIN(JOIN(set, PAIR), deep_copy)(self);
}

static inline void
JOIN(MAP, swap)(MAP* self, MAP* other)
{
    JOIN(JOIN(set, PAIR), swap)(self, other);
}

static inline int
JOIN(MAP, empty)(MAP* self)
{
    return JOIN(JOIN(set, PAIR), empty)(self);
}

static inline size_t
JOIN(MAP, size)(MAP* self)
{
    return self->size;
}

static inline size_t
JOIN(MAP, remove_if)(MAP* self, int _match(K*))
{
    size_t erases = 0;
    MAPIT it = JOIN(JOIN(JOIN(set, PAIR), it), each)(self);
    while (!it.done)
    {
        MAPNODE* current = it.node;
        it.step(&it);
        if (_match(&current->key.first))
        {
            JOIN(JOIN(set, PAIR), erase_node)(self, current);
            erases++;
        }
    }
    return erases;
}

static inline int
JOIN(MAP, equal)(MAP* self, MAP* other, int _equal_value(V*, V*))
{
    if (self->size != other->size)
        return 0;

    MAPIT it_self = JOIN(JOIN(JOIN(set, PAIR), it), each)(self);
    while (!it_self.done)
    {
        MAPNODE* other_node = JOIN(MAP, find)(other, it_self.node->key.first);
        if (!other_node || !_equal_value(&it_self.node->key.second, &other_node->key.second))
            return 0;
        it_self.step(&it_self);
    }
    return 1;
}

static inline MAPIT
JOIN(MAPIT, each)(MAP* self)
{
    return JOIN(JOIN(JOIN(set, PAIR), it), each)(self);
}

static inline MAPIT
JOIN(MAPIT, range)(MAP* self, MAPNODE* begin, MAPNODE* end)
{
    return JOIN(JOIN(JOIN(set, PAIR), it), range)(self, begin, end);
}

static inline MAPNODE*
JOIN(MAP, begin)(MAP* self)
{
    return JOIN(JOIN(set, PAIR), begin)(self);
}

static inline MAPNODE*
JOIN(MAP, end)(MAP* self)
{
    return JOIN(JOIN(set, PAIR), end)(self);
}

#undef K
#undef V
#undef PAIR
#undef MAP
#undef MAPNODE
#undef MAPIT
