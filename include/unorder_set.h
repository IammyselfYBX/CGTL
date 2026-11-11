#ifndef T
#error "Template type T undefined for <unorder_set.h>"
#endif

#include "cgtl.h"

#define HASH JOIN(ust, T)
#define HASHNODE JOIN(HASH, node)
#define HASHIT JOIN(HASH, it)

typedef struct HASHNODE
{
    T key;
    struct HASHNODE* next;
}
HASHNODE;

typedef struct HASH
{
    void (*free)(T*);
    T (*copy)(T*);
    size_t (*hash)(T*);
    int (*equal)(T*, T*);
    HASHNODE** bucket;
    size_t size;
    size_t bucket_count;
}
HASH;

typedef struct HASHIT
{
    void (*step)(struct HASHIT*);
    HASHNODE* end;
    HASHNODE* node;
    T* ref;
    HASHNODE* next;
    HASH* container;
    size_t index;
    int done;
}
HASHIT;

static inline HASHNODE*
JOIN(HASH, begin)(HASH* self)
{
    for(size_t i = 0; i < self->bucket_count; i++)
    {
        HASHNODE* node = self->bucket[i];
        if(node)
            return node;
    }
    return NULL;
}

static inline HASHNODE*
JOIN(HASH, end)(HASH* self)
{
    (void) self;
    return NULL;
}

static inline size_t
JOIN(HASHIT, index)(HASH* self, T value)
{
    return self->hash(&value) % self->bucket_count;
}

static inline void
JOIN(HASHIT, update)(HASHIT* self)
{
    self->node = self->next;
    self->ref = &self->node->key;
    self->next = self->node->next;
}

static inline int
JOIN(HASHIT, scan)(HASHIT* self)
{
    for(size_t i = self->index + 1; i < self->container->bucket_count; i++)
    {
        self->next = self->container->bucket[i];
        if(self->next)
        {
            self->index = i;
            JOIN(HASHIT, update)(self);
            return 1;
        }
    }
    return 0;
}

static inline void
JOIN(HASHIT, step)(HASHIT* self)
{
    if(self->next == JOIN(HASH, end)(self->container))
    {
        if(!JOIN(HASHIT, scan)(self))
            self->done = 1;
    }
    else
        JOIN(HASHIT, update)(self);
}

static inline HASHIT
JOIN(HASHIT, range)(HASH* container, HASHNODE* begin, HASHNODE* end)
{
    static HASHIT zero;
    HASHIT self = zero;
    if(begin)
    {
        self.step = JOIN(HASHIT, step);
        self.node = begin;
        self.ref = &self.node->key;
        self.next = self.node->next;
        self.end = end;
        self.container = container;
        self.index = JOIN(HASHIT, index)(container, *self.ref);
    }
    else
        self.done = 1;
    return self;
}

static inline HASHNODE**
JOIN(HASH, bucket)(HASH* self, T value)
{
    size_t index = JOIN(HASHIT, index)(self, value);
    return &self->bucket[index];
}

static inline int
JOIN(HASH, empty)(HASH* self)
{
    return self->size == 0;
}

static inline HASHIT
JOIN(HASHIT, each)(HASH* a)
{
    return JOIN(HASH, empty)(a)
         ? JOIN(HASHIT, range)(a, NULL, NULL)
         : JOIN(HASHIT, range)(a, JOIN(HASH, begin)(a), JOIN(HASH, end)(a));
}

static inline HASHNODE*
JOIN(HASH, find)(HASH* self, T value)
{
    if(!JOIN(HASH, empty)(self))
    {
        HASHNODE** bucket = JOIN(HASH, bucket)(self, value);
        for(HASHNODE* n = *bucket; n; n = n->next)
            if(self->equal(&value, &n->key))
                return n;
    }
    return NULL;
}

static inline int
JOIN(HASH, equal)(HASH* a, HASH* b)
{
    size_t count_a = 0;
    size_t count_b = 0;
    foreach(HASH, a, it) if(JOIN(HASH, find)(b, *it.ref)) count_a += 1;
    foreach(HASH, b, it) if(JOIN(HASH, find)(a, *it.ref)) count_b += 1;
    return count_a == count_b;
}

static inline void
JOIN(HASH, swap)(HASH* self, HASH* other)
{
    HASH temp = *self;
    *self = *other;
    *other = temp;
}

static inline size_t
JOIN(HASH, closest_prime)(size_t number)
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

static inline HASHNODE*
JOIN(HASHNODE, init)(T value)
{
    HASHNODE* n = (HASHNODE*) malloc(sizeof(HASHNODE));
    n->key = value;
    n->next = NULL;
    return n;
}

static inline void
JOIN(HASHNODE, push)(HASH* self, HASHNODE** bucket, HASHNODE* n)
{
    n->next = *bucket;
    self->size += 1;
    *bucket = n;
}

static inline size_t
JOIN(HASH, bucket_size)(HASH* self, size_t index)
{
    size_t size = 0;
    for(HASHNODE* n = self->bucket[index]; n; n = n->next)
        size += 1;
    return size;
}

static inline float
JOIN(HASH, load_factor)(HASH* self)
{
    return (float) self->size / (float) self->bucket_count;
}

static inline HASH
JOIN(HASH, init)(size_t _hash(T*), int _equal(T*, T*))
{
    static HASH zero;
    HASH self = zero;
    self.hash = _hash;
    self.equal = _equal;
    self.copy = NULL;
    self.free = NULL;
    return self;
}

static inline void
JOIN(HASH, rehash)(HASH* self, size_t desired_count);

static inline void
JOIN(HASH, reserve)(HASH* self, size_t desired_count)
{
    if(self->size > 0)
        JOIN(HASH, rehash)(self, desired_count);
    else
    {
        size_t bucket_count = JOIN(HASH, closest_prime)(desired_count);
        HASHNODE** temp = (HASHNODE**) calloc(bucket_count, sizeof(HASHNODE*));
        for(size_t i = 0; i < self->bucket_count; i++)
            temp[i] = self->bucket[i];
        free(self->bucket);
        self->bucket = temp;
        self->bucket_count = bucket_count;
    }
}

static inline void
JOIN(HASH, rehash)(HASH* self, size_t desired_count)
{
    if(desired_count <= self->size)
        desired_count = self->size + 1;
    size_t expected = JOIN(HASH, closest_prime)(desired_count);
    if(expected != self->bucket_count)
    {
        HASH rehashed = JOIN(HASH, init)(self->hash, self->equal);
        JOIN(HASH, reserve)(&rehashed, desired_count);
        foreach(HASH, self, it)
        {
            HASHNODE** bucket = JOIN(HASH, bucket)(&rehashed, it.node->key);
            JOIN(HASHNODE, push)(&rehashed, bucket, it.node);
        }
        free(self->bucket);
        *self = rehashed;
    }
}

static inline void
JOIN(HASH, free_node)(HASH* self, HASHNODE* n)
{
    if(self->free)
        self->free(&n->key);
    free(n);
    self->size -= 1;
}

static inline void
JOIN(HASH, clear)(HASH* self)
{
    foreach(HASH, self, it)
        JOIN(HASH, free_node)(self, it.node);
    for(size_t i = 0; i < self->bucket_count; i++)
        self->bucket[i] = NULL;
}

static inline void
JOIN(HASH, free)(HASH* self)
{
    JOIN(HASH, clear)(self);
    free(self->bucket);
}

static inline void
JOIN(HASH, insert)(HASH* self, T value)
{
    if(JOIN(HASH, empty)(self))
        JOIN(HASH, rehash)(self, 12);
    if(JOIN(HASH, find)(self, value))
    {
        if(self->free)
            self->free(&value);
    }
    else
    {
        HASHNODE** bucket = JOIN(HASH, bucket)(self, value);
        JOIN(HASHNODE, push)(self, bucket, JOIN(HASHNODE, init)(value));
        if(self->size > self->bucket_count)
            JOIN(HASH, rehash)(self, 2 * self->bucket_count);
    }
}

static inline size_t
JOIN(HASH, count)(HASH* self, T value)
{
    return JOIN(HASH, find)(self, value) ? 1 : 0;
}

static inline void
JOIN(HASH, linked_erase)(HASH* self, HASHNODE** bucket, HASHNODE* n, HASHNODE* prev, HASHNODE* next)
{
    JOIN(HASH, free_node)(self, n);
    if(prev)
        prev->next = next;
    else
        *bucket = next;
}

static inline void
JOIN(HASH, erase)(HASH* self, T value)
{
    if(!JOIN(HASH, empty)(self))
    {
        HASHNODE** bucket = JOIN(HASH, bucket)(self, value);
        HASHNODE* prev = NULL;
        HASHNODE* n = *bucket;
        while(n)
        {
            HASHNODE* next = n->next;
            if(self->equal(&n->key, &value))
            {
                JOIN(HASH, linked_erase)(self, bucket, n, prev, next);
                break;
            }
            else
                prev = n;
            n = next;
        }
    }
}

static inline size_t
JOIN(HASH, remove_if)(HASH* self, int _match(T*))
{
    size_t erases = 0;
    for(size_t i = 0; i < self->bucket_count; i++)
    {
        HASHNODE** bucket = &self->bucket[i];
        HASHNODE* prev = NULL;
        HASHNODE* n = *bucket;
        while(n)
        {
            HASHNODE* next = n->next;
            if(_match(&n->key))
            {
                JOIN(HASH, linked_erase)(self, bucket, n, prev, next);
                erases += 1;
            }
            else
                prev = n;
            n = next;
        }
    }
    return erases;
}

static inline HASH
JOIN(HASH, shallow_copy)(HASH* self)
{
    HASH other = JOIN(HASH, init)(self->hash, self->equal);

    other.copy = self->copy;
    other.free = NULL;

    foreach(HASH, self, it)
        JOIN(HASH, insert)(&other, *it.ref);
    return other;
}

static inline HASH
JOIN(HASH, deep_copy)(HASH* self)
{
    HASH other = JOIN(HASH, init)(self->hash, self->equal);

    other.copy = self->copy;
    other.free = self->free;

    foreach(HASH, self, it) {
        if (other.copy) {
            JOIN(HASH, insert)(&other, other.copy(it.ref));
        } else {
            JOIN(HASH, insert)(&other, *it.ref);
        }
    }
    return other;
}

#undef T
#undef HASH
#undef HASHNODE
#undef HASHIT
