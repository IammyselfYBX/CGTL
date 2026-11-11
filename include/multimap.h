#ifndef K
#error "Template key type K undefined for <multimap.h>"
#endif

#ifndef V
#error "Template value type V undefined for <multimap.h>"
#endif

#include "cgtl.h"

#define PAIR JOIN(JOIN(pair, K), V)
#define MULTIMAP JOIN(JOIN(multimap, K), V)
#define MULTIMAPNODE JOIN(MULTIMAP, node)
#define MULTIMAPIT JOIN(MULTIMAP, it)

typedef struct PAIR
{
    K first;
    V second;
} PAIR;

#define ALLOW_DUPLICATES
#define T PAIR
#define HOLD
#include "set.h"
#undef T

typedef JOIN(set, PAIR) MULTIMAP;
typedef JOIN(JOIN(set, PAIR), node) MULTIMAPNODE;
typedef JOIN(JOIN(set, PAIR), it) MULTIMAPIT;

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

static inline MULTIMAP
JOIN(MULTIMAP, init)(int _compare(K*, K*))
{
    (void)_compare;
    return JOIN(JOIN(set, PAIR), init)(JOIN(PAIR, compare_key));
}

static inline MULTIMAPNODE*
JOIN(MULTIMAP, insert)(MULTIMAP* self, K key, V value)
{
    PAIR pair;
    pair.first = key;
    pair.second = value;

    return JOIN(JOIN(set, PAIR), insert)(self, self->copy ? self->copy(&pair) : JOIN(PAIR, shallow_copy)(&pair));
}

static inline MULTIMAPNODE*
JOIN(MULTIMAP, find)(MULTIMAP* self, K key)
{
    PAIR temp;
    temp.first = key;
    return JOIN(JOIN(set, PAIR), find)(self, temp);
}

static inline void
JOIN(MULTIMAP, erase)(MULTIMAP* self, K key)
{
    PAIR temp;
    temp.first = key;
    JOIN(JOIN(set, PAIR), erase)(self, temp);
}

static inline size_t
JOIN(MULTIMAP, erase_all)(MULTIMAP* self, K key)
{
    PAIR temp;
    temp.first = key;
    return JOIN(JOIN(set, PAIR), erase_all)(self, temp);
}

static inline size_t
JOIN(MULTIMAP, count)(MULTIMAP* self, K key)
{
    PAIR temp;
    temp.first = key;
    return JOIN(JOIN(set, PAIR), count_all)(self, temp);
}

static inline void
JOIN(MULTIMAP, equal_range)(MULTIMAP* self, K key, MULTIMAPNODE** begin, MULTIMAPNODE** end)
{
    PAIR temp;
    temp.first = key;
    JOIN(JOIN(set, PAIR), equal_range)(self, temp, begin, end);
}

static inline MULTIMAPNODE*
JOIN(MULTIMAP, lower_bound)(MULTIMAP* self, K key)
{
    PAIR temp;
    temp.first = key;
    return JOIN(JOIN(set, PAIR), lower_bound)(self, temp);
}

static inline MULTIMAPNODE*
JOIN(MULTIMAP, upper_bound)(MULTIMAP* self, K key)
{
    PAIR temp;
    temp.first = key;
    return JOIN(JOIN(set, PAIR), upper_bound)(self, temp);
}

static inline void
JOIN(MULTIMAP, clear)(MULTIMAP* self)
{
    JOIN(JOIN(set, PAIR), clear)(self);
}

static inline void
JOIN(MULTIMAP, free)(MULTIMAP* self)
{
    JOIN(JOIN(set, PAIR), free)(self);
}

static inline MULTIMAP
JOIN(MULTIMAP, shallow_copy)(MULTIMAP* self)
{
    return JOIN(JOIN(set, PAIR), shallow_copy)(self);
}

static inline MULTIMAP
JOIN(MULTIMAP, deep_copy)(MULTIMAP* self)
{
    return JOIN(JOIN(set, PAIR), deep_copy)(self);
}

static inline void
JOIN(MULTIMAP, swap)(MULTIMAP* self, MULTIMAP* other)
{
    JOIN(JOIN(set, PAIR), swap)(self, other);
}

static inline int
JOIN(MULTIMAP, empty)(MULTIMAP* self)
{
    return JOIN(JOIN(set, PAIR), empty)(self);
}

static inline size_t
JOIN(MULTIMAP, size)(MULTIMAP* self)
{
    return self->size;
}

static inline size_t
JOIN(MULTIMAP, remove_if)(MULTIMAP* self, int _match(K*))
{
    size_t erases = 0;
    MULTIMAPIT it = JOIN(JOIN(JOIN(set, PAIR), it), each)(self);
    while (!it.done)
    {
        MULTIMAPNODE* current = it.node;
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
JOIN(MULTIMAP, equal)(MULTIMAP* self, MULTIMAP* other, int _equal_value(V*, V*))
{
    if (self->size != other->size)
        return 0;

    MULTIMAPIT it_self = JOIN(JOIN(JOIN(set, PAIR), it), each)(self);
    MULTIMAPIT it_other = JOIN(JOIN(JOIN(set, PAIR), it), each)(other);

    while (!it_self.done && !it_other.done)
    {

        if (self->compare(&it_self.node->key, &it_other.node->key) != 0)
            return 0;

        if (!_equal_value(&it_self.node->key.second, &it_other.node->key.second))
            return 0;
        it_self.step(&it_self);
        it_other.step(&it_other);
    }
    return 1;
}

static inline MULTIMAPIT
JOIN(MULTIMAPIT, each)(MULTIMAP* self)
{
    return JOIN(JOIN(JOIN(set, PAIR), it), each)(self);
}

static inline MULTIMAPIT
JOIN(MULTIMAPIT, range)(MULTIMAP* self, MULTIMAPNODE* begin, MULTIMAPNODE* end)
{
    return JOIN(JOIN(JOIN(set, PAIR), it), range)(self, begin, end);
}

static inline MULTIMAPNODE*
JOIN(MULTIMAP, begin)(MULTIMAP* self)
{
    return JOIN(JOIN(set, PAIR), begin)(self);
}

static inline MULTIMAPNODE*
JOIN(MULTIMAP, end)(MULTIMAP* self)
{
    return JOIN(JOIN(set, PAIR), end)(self);
}

#undef ALLOW_DUPLICATES
#undef K
#undef V
#undef PAIR
#undef MULTIMAP
#undef MULTIMAPNODE
#undef MULTIMAPIT
