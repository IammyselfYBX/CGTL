#ifndef T
#error "Template type T undefined for <unordered_multiset.h>"
#endif

#include "cgtl.h"
#include "iterator.h"

#define UMSET JOIN(umst, T)
#define UMSETNODE JOIN(UMSET, node)
#define UMSETIT JOIN(UMSET, it)

#define CGTL_ALGO_PREFIX JOIN(UMSET, algo)
#include "algorithm.h"
#undef CGTL_ALGO_PREFIX

typedef struct UMSETNODE
{
    T key;
    size_t count;
    struct UMSETNODE* next;
}
UMSETNODE;

typedef struct UMSET
{
    void (*free)(T*);
    T (*copy)(T*);
    size_t (*hash)(T*);
    int (*equal)(T*, T*);
    UMSETNODE** bucket;
    size_t size;
    size_t unique_size;
    size_t bucket_count;
}
UMSET;

typedef struct UMSETIT
{
    void (*step)(struct UMSETIT*);
    UMSETNODE* end;
    UMSETNODE* node;
    T* ref;
    UMSETNODE* next;
    UMSET* container;
    size_t index;
    size_t current_count;
    int done;
}
UMSETIT;

static inline UMSETNODE*
JOIN(UMSET, begin)(UMSET* self)
{
    for(size_t i = 0; i < self->bucket_count; i++)
    {
        UMSETNODE* node = self->bucket[i];
        if(node)
            return node;
    }
    return NULL;
}

static inline UMSETNODE*
JOIN(UMSET, end)(UMSET* self)
{
    (void) self;
    return NULL;
}

static inline size_t
JOIN(UMSETIT, index)(UMSET* self, T value)
{
    return self->hash(&value) % self->bucket_count;
}

static inline void
JOIN(UMSETIT, update)(UMSETIT* self)
{
    self->node = self->next;
    self->ref = &self->node->key;
    self->next = self->node->next;
    self->current_count = self->node->count;
}

static inline int
JOIN(UMSETIT, scan)(UMSETIT* self)
{
    for(size_t i = self->index + 1; i < self->container->bucket_count; i++)
    {
        self->next = self->container->bucket[i];
        if(self->next)
        {
            self->index = i;
            JOIN(UMSETIT, update)(self);
            return 1;
        }
    }
    return 0;
}

static inline void
JOIN(UMSETIT, step)(UMSETIT* self)
{
    if(self->current_count > 1)
    {

        self->current_count--;
    }
    else
    {

        if(self->next == JOIN(UMSET, end)(self->container))
        {
            if(!JOIN(UMSETIT, scan)(self))
                self->done = 1;
        }
        else
            JOIN(UMSETIT, update)(self);
    }
}

static inline UMSETIT
JOIN(UMSETIT, range)(UMSET* container, UMSETNODE* begin, UMSETNODE* end)
{
    static UMSETIT zero;
    UMSETIT self = zero;
    if(begin)
    {
        self.step = JOIN(UMSETIT, step);
        self.node = begin;
        self.ref = &self.node->key;
        self.next = self.node->next;
        self.end = end;
        self.container = container;
        self.index = JOIN(UMSETIT, index)(container, *self.ref);
        self.current_count = self.node->count;
    }
    else
        self.done = 1;
    return self;
}

static inline UMSETNODE**
JOIN(UMSET, bucket)(UMSET* self, T value)
{
    size_t index = JOIN(UMSETIT, index)(self, value);
    return &self->bucket[index];
}

static inline int
JOIN(UMSET, empty)(UMSET* self)
{
    return self->size == 0;
}

static inline UMSETIT
JOIN(UMSETIT, each)(UMSET* a)
{
    return JOIN(UMSET, empty)(a)
         ? JOIN(UMSETIT, range)(a, NULL, NULL)
         : JOIN(UMSETIT, range)(a, JOIN(UMSET, begin)(a), JOIN(UMSET, end)(a));
}

static inline void
JOIN(UMSETIT, base_step)(cgtl_iter* it)
{
    if(it->done)
        return;

    if(it->repeat > 1)
    {
        it->repeat--;
        return;
    }

    UMSET* container = (UMSET*)it->container;
    UMSETNODE* node = (UMSETNODE*)it->cur;

    if(node && node->next)
    {
        node = node->next;
        it->cur = node;
        it->ref = &node->key;
        it->repeat = node->count;
        return;
    }

    for(size_t i = it->index + 1; i < container->bucket_count; i++)
    {
        node = container->bucket[i];
        if(node)
        {
            it->index = i;
            it->cur = node;
            it->ref = &node->key;
            it->repeat = node->count;
            return;
        }
    }
    it->done = 1;
}

static inline int
JOIN(UMSETIT, base_equal)(const cgtl_iter* a, const cgtl_iter* b)
{
    if(a->done || b->done)
        return a->done && b->done;
    return a->container == b->container
        && a->cur == b->cur
        && a->repeat == b->repeat;
}

static const cgtl_iter_ops JOIN(UMSETIT, base_ops) = {
    JOIN(UMSETIT, base_step),
    JOIN(UMSETIT, base_equal),
    NULL,
    NULL
};

static inline cgtl_iter
JOIN(UMSET, iter_end)(UMSET* self)
{
    cgtl_iter it = {0};
    it.done = 1;
    it.container = self;
    it.index = self->bucket_count;
    it.index_end = self->bucket_count;
    it.category = CGTL_ITER_FORWARD;
    it.ops = &JOIN(UMSETIT, base_ops);
    return it;
}

static inline cgtl_iter
JOIN(UMSET, iter_begin)(UMSET* self)
{
    if(JOIN(UMSET, empty)(self))
        return JOIN(UMSET, iter_end)(self);

    UMSETNODE* node = JOIN(UMSET, begin)(self);
    cgtl_iter it = {0};
    it.cur = node;
    it.ref = &node->key;
    it.container = self;
    it.index = JOIN(UMSETIT, index)(self, node->key);
    it.index_end = self->bucket_count;
    it.repeat = node->count;
    it.category = CGTL_ITER_FORWARD;
    it.ops = &JOIN(UMSETIT, base_ops);
    return it;
}

static inline cgtl_range
JOIN(UMSET, iter_range)(UMSET* self)
{
    cgtl_range range;
    range.first = JOIN(UMSET, iter_begin)(self);
    range.last = JOIN(UMSET, iter_end)(self);
    return range;
}

static inline T
JOIN(UMSET, implicit_copy)(T* self)
{
    return *self;
}

static inline UMSETNODE*
JOIN(UMSET, find)(UMSET* self, T value)
{
    if(!JOIN(UMSET, empty)(self))
    {
        UMSETNODE** bucket = JOIN(UMSET, bucket)(self, value);
        for(UMSETNODE* n = *bucket; n; n = n->next)
            if(self->equal(&value, &n->key))
                return n;
    }
    return NULL;
}

static inline int
JOIN(UMSET, equal)(UMSET* a, UMSET* b)
{
    if(a->size != b->size || a->unique_size != b->unique_size)
        return 0;

    foreach(UMSET, a, it) {
        UMSETNODE* node_b = JOIN(UMSET, find)(b, *it.ref);
        if(!node_b || node_b->count != it.node->count)
            return 0;
    }
    return 1;
}

static inline void
JOIN(UMSET, swap)(UMSET* self, UMSET* other)
{
    UMSET temp = *self;
    *self = *other;
    *other = temp;
}

static inline size_t
JOIN(UMSET, closest_prime)(size_t number)
{
    static uint32_t primes[] = {
        2, 3, 5, 7, 11,
        13, 17, 19, 23, 29, 31,
        37, 41, 43, 47, 53, 59,
        61, 67, 71, 73, 79, 83,
        89, 97, 103, 109, 113, 127,
        137, 139, 149, 157, 167, 179,
        193, 199, 211, 227, 241, 257,
        277, 293, 313, 337, 359, 383,
        409, 439, 467, 503, 541, 577,
        619, 661, 709, 761, 823, 887,
        953, 1031, 1109, 1193, 1289, 1381,
        1493, 1613, 1741, 1879, 2029, 2179,
        2357, 2549, 2753, 2971, 3209, 3469,
        3739, 4027, 4349, 4703, 5087, 5503,
        5953, 6427, 6949, 7517, 8123, 8783,
        9497, 10273, 11113, 12011, 12983, 14033,
        15173, 16411, 17749, 19183, 20753, 22447,
        24281, 26267, 28411, 30727, 33223, 35933,
        38873, 42043, 45481, 49201, 53201, 57557,
        62233, 67307, 72817, 78779, 85229, 92203,
        99733, 107897, 116731, 126271, 136607, 147793,
        159871, 172933, 187091, 202409, 218971, 236897,
        256279, 277261, 299951, 324503, 351061, 379787,
        410857, 444487, 480881, 520241, 562841, 608903,
        658753, 712697, 771049, 834181, 902483, 976369,
        1056323, 1142821, 1236397, 1337629, 1447153, 1565659,
        1693859, 1832561, 1982627, 2144977, 2320627, 2510653,
        2716249, 2938679, 3179303, 3439651, 3721303, 4026031,
        4355707, 4712381, 5098259, 5515729, 5967347, 6456007,
        6984629, 7556579, 8175383, 8844859, 9569143, 10352717,
        11200489, 12117689, 13109983, 14183539, 15345007, 16601593,
        17961079, 19431899, 21023161, 22744717, 24607243, 26622317,
        28802401, 31160981, 33712729, 36473443, 39460231, 42691603,
        46187573, 49969847, 54061849, 58488943, 63278561, 68460391,
        74066549, 80131819, 86693767, 93793069, 101473717, 109783337,
        118773397, 128499677, 139022417, 150406843, 162723577, 176048909,
        190465427, 206062531, 222936881, 241193053, 260944219, 282312799,
        305431229, 330442829, 357502601, 386778277, 418451333, 452718089,
        489790921, 529899637, 573292817, 620239453, 671030513, 725980837,
        785430967, 849749479, 919334987, 994618837, 1076067617, 1164186217,
        1259520799, 1362662261, 1474249943, 1594975441, 1725587117,
    };
    size_t min = primes[0];
    if(number < min)
        return min;
    size_t size = len(primes);
    for(size_t i = 0; i < size - 1; i++)
    {
        size_t a = primes[i + 0];
        size_t b = primes[i + 1];
        if(number >= a && number <= b)
            return number == a ? a : b;
    }
    return primes[size - 1];
}

static inline UMSETNODE*
JOIN(UMSETNODE, init)(T value)
{
    UMSETNODE* n = (UMSETNODE*) malloc(sizeof(UMSETNODE));
    n->key = value;
    n->count = 1;
    n->next = NULL;
    return n;
}

static inline void
JOIN(UMSETNODE, push)(UMSET* self, UMSETNODE** bucket, UMSETNODE* n)
{
    n->next = *bucket;
    self->size += n->count;
    self->unique_size += 1;
    *bucket = n;
}

static inline size_t
JOIN(UMSET, bucket_size)(UMSET* self, size_t index)
{
    size_t size = 0;
    for(UMSETNODE* n = self->bucket[index]; n; n = n->next)
        size += n->count;
    return size;
}

static inline float
JOIN(UMSET, load_factor)(UMSET* self)
{
    return (float) self->unique_size / (float) self->bucket_count;
}

static inline UMSET
JOIN(UMSET, init)(size_t _hash(T*), int _equal(T*, T*))
{
    static UMSET zero;
    UMSET self = zero;
    self.hash = _hash;
    self.equal = _equal;
#ifdef CGTL_IC
#undef CGTL_IC
    self.copy = JOIN(UMSET, implicit_copy);
#else
    self.free = JOIN(T, free);
    self.copy = JOIN(T, copy);
#endif
    return self;
}

static inline void
JOIN(UMSET, rehash)(UMSET* self, size_t desired_count);

static inline void
JOIN(UMSET, reserve)(UMSET* self, size_t desired_count)
{
    if(self->unique_size > 0)
        JOIN(UMSET, rehash)(self, desired_count);
    else
    {
        size_t bucket_count = JOIN(UMSET, closest_prime)(desired_count);
        UMSETNODE** temp = (UMSETNODE**) calloc(bucket_count, sizeof(UMSETNODE*));
        for(size_t i = 0; i < self->bucket_count; i++)
            temp[i] = self->bucket[i];
        free(self->bucket);
        self->bucket = temp;
        self->bucket_count = bucket_count;
    }
}

static inline void
JOIN(UMSET, rehash)(UMSET* self, size_t desired_count)
{
    if(desired_count <= self->unique_size)
        desired_count = self->unique_size + 1;
    size_t expected = JOIN(UMSET, closest_prime)(desired_count);
    if(expected != self->bucket_count)
    {
        UMSET rehashed = JOIN(UMSET, init)(self->hash, self->equal);
        JOIN(UMSET, reserve)(&rehashed, desired_count);
        foreach(UMSET, self, it)
        {
            UMSETNODE** bucket = JOIN(UMSET, bucket)(&rehashed, it.node->key);
            JOIN(UMSETNODE, push)(&rehashed, bucket, it.node);
        }
        free(self->bucket);
        *self = rehashed;
    }
}

static inline void
JOIN(UMSET, free_node)(UMSET* self, UMSETNODE* n)
{
    if(self->free)
        self->free(&n->key);
    self->size -= n->count;
    self->unique_size -= 1;
    free(n);
}

static inline void
JOIN(UMSET, clear)(UMSET* self)
{
    for(size_t i = 0; i < self->bucket_count; i++)
    {
        UMSETNODE* node = self->bucket[i];
        while(node)
        {
            UMSETNODE* next = node->next;
            JOIN(UMSET, free_node)(self, node);
            node = next;
        }
        self->bucket[i] = NULL;
    }
}

static inline void
JOIN(UMSET, free)(UMSET* self)
{
    JOIN(UMSET, clear)(self);
    free(self->bucket);
}

static inline void
JOIN(UMSET, insert)(UMSET* self, T value)
{
    if(JOIN(UMSET, empty)(self))
        JOIN(UMSET, rehash)(self, 12);

    UMSETNODE* existing = JOIN(UMSET, find)(self, value);
    if(existing)
    {

        existing->count++;
        self->size++;
        if(self->free)
            self->free(&value);
    }
    else
    {

        UMSETNODE** bucket = JOIN(UMSET, bucket)(self, value);
        JOIN(UMSETNODE, push)(self, bucket, JOIN(UMSETNODE, init)(value));
        if(self->unique_size > self->bucket_count)
            JOIN(UMSET, rehash)(self, 2 * self->bucket_count);
    }
}

static inline size_t
JOIN(UMSET, count)(UMSET* self, T value)
{
    UMSETNODE* node = JOIN(UMSET, find)(self, value);
    return node ? node->count : 0;
}

static inline void
JOIN(UMSET, linked_erase)(UMSET* self, UMSETNODE** bucket, UMSETNODE* n, UMSETNODE* prev, UMSETNODE* next)
{
    JOIN(UMSET, free_node)(self, n);
    if(prev)
        prev->next = next;
    else
        *bucket = next;
}

static inline void
JOIN(UMSET, erase)(UMSET* self, T value)
{
    if(!JOIN(UMSET, empty)(self))
    {
        UMSETNODE** bucket = JOIN(UMSET, bucket)(self, value);
        UMSETNODE* prev = NULL;
        UMSETNODE* n = *bucket;
        while(n)
        {
            UMSETNODE* next = n->next;
            if(self->equal(&n->key, &value))
            {
                if(n->count > 1)
                {

                    n->count--;
                    self->size--;
                }
                else
                {

                    JOIN(UMSET, linked_erase)(self, bucket, n, prev, next);
                }
                break;
            }
            else
                prev = n;
            n = next;
        }
    }
}

static inline size_t
JOIN(UMSET, erase_all)(UMSET* self, T value)
{
    size_t erased_count = 0;
    if(!JOIN(UMSET, empty)(self))
    {
        UMSETNODE** bucket = JOIN(UMSET, bucket)(self, value);
        UMSETNODE* prev = NULL;
        UMSETNODE* n = *bucket;
        while(n)
        {
            UMSETNODE* next = n->next;
            if(self->equal(&n->key, &value))
            {
                erased_count = n->count;
                JOIN(UMSET, linked_erase)(self, bucket, n, prev, next);
                break;
            }
            else
                prev = n;
            n = next;
        }
    }
    return erased_count;
}

static inline size_t
JOIN(UMSET, remove_if)(UMSET* self, int _match(T*))
{
    size_t erases = 0;
    for(size_t i = 0; i < self->bucket_count; i++)
    {
        UMSETNODE** bucket = &self->bucket[i];
        UMSETNODE* prev = NULL;
        UMSETNODE* n = *bucket;
        while(n)
        {
            UMSETNODE* next = n->next;
            if(_match(&n->key))
            {
                erases += n->count;
                JOIN(UMSET, linked_erase)(self, bucket, n, prev, next);
            }
            else
                prev = n;
            n = next;
        }
    }
    return erases;
}

static inline UMSET
JOIN(UMSET, copy)(UMSET* self)
{
    UMSET other = JOIN(UMSET, init)(self->hash, self->equal);
    foreach(UMSET, self, it) {

        for(size_t i = 0; i < it.node->count; i++) {
            JOIN(UMSET, insert)(&other, self->copy(it.ref));
        }
    }
    return other;
}

#undef T
#undef UMSET
#undef UMSETNODE
#undef UMSETIT
