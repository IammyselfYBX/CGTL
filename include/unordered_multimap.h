#ifndef K
#error "Template key type K undefined for <unordered_multimap.h>"
#endif

#ifndef V
#error "Template value type V undefined for <unordered_multimap.h>"
#endif

#include "cgtl.h"
#include "iterator.h"
#include <string.h>

#define UMMAP JOIN(JOIN(ummap, K), V)
#define UMMAPNODE JOIN(UMMAP, node)
#define UMMAPIT JOIN(UMMAP, it)

#define T K
#define CGTL_ALGO_PREFIX JOIN(UMMAP, algo)
#include "algorithm.h"
#undef T
#undef CGTL_ALGO_PREFIX

typedef struct UMMAPNODE
{
    K key;
    V value;
    struct UMMAPNODE* next;
}
UMMAPNODE;

typedef struct UMMAP
{
    K (*copy_key)(K*);
    V (*copy_value)(V*);
    void (*free_key)(K*);
    void (*free_value)(V*);
    size_t (*hash)(K*);
    int (*equal)(K*, K*);
    UMMAPNODE** bucket;
    size_t size;
    size_t bucket_count;
}
UMMAP;

typedef struct UMMAPIT
{
    void (*step)(struct UMMAPIT*);
    UMMAPNODE* end;
    UMMAPNODE* node;
    K* key_ref;
    V* value_ref;
    UMMAPNODE* next;
    UMMAP* container;
    size_t index;
    int done;
}
UMMAPIT;

static inline UMMAPNODE*
JOIN(UMMAP, begin)(UMMAP* self)
{
    for(size_t i = 0; i < self->bucket_count; i++)
    {
        UMMAPNODE* node = self->bucket[i];
        if(node)
            return node;
    }
    return NULL;
}

static inline UMMAPNODE*
JOIN(UMMAP, end)(UMMAP* self)
{
    (void) self;
    return NULL;
}

static inline size_t
JOIN(UMMAPIT, index)(UMMAP* self, K key)
{
    return self->hash(&key) % self->bucket_count;
}

static inline void
JOIN(UMMAPIT, update)(UMMAPIT* self)
{
    self->node = self->next;
    self->key_ref = &self->node->key;
    self->value_ref = &self->node->value;
    self->next = self->node->next;
}

static inline int
JOIN(UMMAPIT, scan)(UMMAPIT* self)
{
    for(size_t i = self->index + 1; i < self->container->bucket_count; i++)
    {
        self->next = self->container->bucket[i];
        if(self->next)
        {
            self->index = i;
            JOIN(UMMAPIT, update)(self);
            return 1;
        }
    }
    return 0;
}

static inline void
JOIN(UMMAPIT, step)(UMMAPIT* self)
{
    if(self->next == JOIN(UMMAP, end)(self->container))
    {
        if(!JOIN(UMMAPIT, scan)(self))
            self->done = 1;
    }
    else
        JOIN(UMMAPIT, update)(self);
}

static inline UMMAPIT
JOIN(UMMAPIT, range)(UMMAP* container, UMMAPNODE* begin, UMMAPNODE* end)
{
    static UMMAPIT zero;
    UMMAPIT self = zero;
    if(begin)
    {
        self.step = JOIN(UMMAPIT, step);
        self.node = begin;
        self.key_ref = &self.node->key;
        self.value_ref = &self.node->value;
        self.next = self.node->next;
        self.end = end;
        self.container = container;
        self.index = JOIN(UMMAPIT, index)(container, *self.key_ref);
    }
    else
        self.done = 1;
    return self;
}

static inline UMMAPNODE**
JOIN(UMMAP, bucket)(UMMAP* self, K key)
{
    size_t index = JOIN(UMMAPIT, index)(self, key);
    return &self->bucket[index];
}

static inline int
JOIN(UMMAP, empty)(UMMAP* self)
{
    return self->size == 0;
}

static inline UMMAPIT
JOIN(UMMAPIT, each)(UMMAP* a)
{
    return JOIN(UMMAP, empty)(a)
         ? JOIN(UMMAPIT, range)(a, NULL, NULL)
         : JOIN(UMMAPIT, range)(a, JOIN(UMMAP, begin)(a), JOIN(UMMAP, end)(a));
}

static inline void
JOIN(UMMAPIT, base_step)(cgtl_iter* it)
{
    if(it->done)
        return;

    UMMAP* container = (UMMAP*)it->container;
    UMMAPNODE* node = (UMMAPNODE*)it->cur;

    if(node && node->next)
    {
        node = node->next;
        it->cur = node;
        it->ref = &node->key;
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
            return;
        }
    }
    it->done = 1;
}

static inline int
JOIN(UMMAPIT, base_equal)(const cgtl_iter* a, const cgtl_iter* b)
{
    if(a->done || b->done)
        return a->done && b->done;
    return a->container == b->container && a->cur == b->cur;
}

static const cgtl_iter_ops JOIN(UMMAPIT, base_ops) = {
    JOIN(UMMAPIT, base_step),
    JOIN(UMMAPIT, base_equal),
    NULL,
    NULL
};

static inline cgtl_iter
JOIN(UMMAP, iter_end)(UMMAP* self)
{
    cgtl_iter it = {0};
    it.done = 1;
    it.container = self;
    it.index = self->bucket_count;
    it.index_end = self->bucket_count;
    it.category = CGTL_ITER_FORWARD;
    it.ops = &JOIN(UMMAPIT, base_ops);
    return it;
}

static inline cgtl_iter
JOIN(UMMAP, iter_begin)(UMMAP* self)
{
    if(JOIN(UMMAP, empty)(self))
        return JOIN(UMMAP, iter_end)(self);

    UMMAPNODE* node = JOIN(UMMAP, begin)(self);
    cgtl_iter it = {0};
    it.cur = node;
    it.ref = &node->key;
    it.container = self;
    it.index = JOIN(UMMAPIT, index)(self, node->key);
    it.index_end = self->bucket_count;
    it.category = CGTL_ITER_FORWARD;
    it.ops = &JOIN(UMMAPIT, base_ops);
    return it;
}

static inline cgtl_range
JOIN(UMMAP, iter_range)(UMMAP* self)
{
    cgtl_range range;
    range.first = JOIN(UMMAP, iter_begin)(self);
    range.last = JOIN(UMMAP, iter_end)(self);
    return range;
}

static inline size_t
JOIN(UMMAP, closest_prime)(size_t number)
{
    static uint32_t primes[] = {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97
    };
    size_t size = sizeof(primes) / sizeof(primes[0]);
    for(size_t i = 0; i < size; i++)
    {
        if(primes[i] >= number)
            return primes[i];
    }
    return primes[size - 1];
}

static inline UMMAP
JOIN(UMMAP, init)(size_t _hash(K*), int _equal(K*, K*))
{
    static UMMAP zero;
    UMMAP self = zero;
    self.hash = _hash;
    self.equal = _equal;

    self.copy_key = NULL;
    self.copy_value = NULL;
    self.free_key = NULL;
    self.free_value = NULL;
    return self;
}

static inline UMMAPNODE*
JOIN(UMMAPNODE, init)(K key, V value)
{
    UMMAPNODE* node = (UMMAPNODE*) malloc(sizeof(UMMAPNODE));
    node->key = key;
    node->value = value;
    node->next = NULL;
    return node;
}

static inline void
JOIN(UMMAP, reserve)(UMMAP* self, size_t desired_count)
{
    if(desired_count > 0) {
        size_t bucket_count = JOIN(UMMAP, closest_prime)(desired_count);
        if(bucket_count > self->bucket_count) {
            UMMAPNODE** temp = (UMMAPNODE**) calloc(bucket_count, sizeof(UMMAPNODE*));
            if(self->bucket) {

                for(size_t i = 0; i < self->bucket_count; i++) {
                    UMMAPNODE* node = self->bucket[i];
                    while(node) {
                        UMMAPNODE* next = node->next;
                        size_t new_index = self->hash(&node->key) % bucket_count;
                        node->next = temp[new_index];
                        temp[new_index] = node;
                        node = next;
                    }
                }
                free(self->bucket);
            }
            self->bucket = temp;
            self->bucket_count = bucket_count;
        }
    }
}

static inline void
JOIN(UMMAP, insert)(UMMAP* self, K key, V value)
{
    if(JOIN(UMMAP, empty)(self))
        JOIN(UMMAP, reserve)(self, 12);

    UMMAPNODE** bucket = JOIN(UMMAP, bucket)(self, key);
    UMMAPNODE* node = JOIN(UMMAPNODE, init)(key, value);
    node->next = *bucket;
    *bucket = node;
    self->size += 1;

    if(self->size > self->bucket_count)
        JOIN(UMMAP, reserve)(self, 2 * self->bucket_count);
}

static inline UMMAPNODE*
JOIN(UMMAP, find)(UMMAP* self, K key)
{
    if(!JOIN(UMMAP, empty)(self))
    {
        UMMAPNODE** bucket = JOIN(UMMAP, bucket)(self, key);
        for(UMMAPNODE* node = *bucket; node; node = node->next)
            if(self->equal(&key, &node->key))
                return node;
    }
    return NULL;
}

static inline size_t
JOIN(UMMAP, count)(UMMAP* self, K key)
{
    size_t count = 0;
    if(!JOIN(UMMAP, empty)(self))
    {
        UMMAPNODE** bucket = JOIN(UMMAP, bucket)(self, key);
        for(UMMAPNODE* node = *bucket; node; node = node->next)
            if(self->equal(&key, &node->key))
                count++;
    }
    return count;
}

static inline V*
JOIN(UMMAP, equal_range)(UMMAP* self, K key, size_t* count)
{
    *count = 0;
    UMMAPNODE* node = JOIN(UMMAP, find)(self, key);
    if (node) {
        *count = 1;
        return &node->value;
    }
    return NULL;
}

static inline int
JOIN(UMMAP, erase)(UMMAP* self, K key, V value)
{
    if(!JOIN(UMMAP, empty)(self))
    {
        UMMAPNODE** bucket = JOIN(UMMAP, bucket)(self, key);
        UMMAPNODE* prev = NULL;
        UMMAPNODE* node = *bucket;
        while(node)
        {
            UMMAPNODE* next = node->next;
            if(self->equal(&node->key, &key))
            {

                int value_match = (memcmp(&node->value, &value, sizeof(V)) == 0);
                if(value_match) {
                    if(self->free_key) self->free_key(&node->key);
                    if(self->free_value) self->free_value(&node->value);

                    if(prev)
                        prev->next = next;
                    else
                        *bucket = next;
                    free(node);
                    self->size--;
                    return 1;
                }
            }
            prev = node;
            node = next;
        }
    }
    return 0;
}

static inline size_t
JOIN(UMMAP, erase_all)(UMMAP* self, K key)
{
    size_t erased = 0;
    if(!JOIN(UMMAP, empty)(self))
    {
        UMMAPNODE** bucket = JOIN(UMMAP, bucket)(self, key);
        UMMAPNODE* prev = NULL;
        UMMAPNODE* node = *bucket;
        while(node)
        {
            UMMAPNODE* next = node->next;
            if(self->equal(&node->key, &key))
            {
                if(self->free_key) self->free_key(&node->key);
                if(self->free_value) self->free_value(&node->value);

                if(prev)
                    prev->next = next;
                else
                    *bucket = next;
                free(node);
                self->size--;
                erased++;
                node = next;
            }
            else
            {
                prev = node;
                node = next;
            }
        }
    }
    return erased;
}

static inline void
JOIN(UMMAP, clear)(UMMAP* self)
{
    for(size_t i = 0; i < self->bucket_count; i++)
    {
        UMMAPNODE* node = self->bucket[i];
        while(node)
        {
            UMMAPNODE* next = node->next;
            if(self->free_key) self->free_key(&node->key);
            if(self->free_value) self->free_value(&node->value);
            free(node);
            node = next;
        }
        self->bucket[i] = NULL;
    }
    self->size = 0;
}

static inline void
JOIN(UMMAP, shallow_free)(UMMAP* self)
{

    for(size_t i = 0; i < self->bucket_count; i++)
    {
        UMMAPNODE* node = self->bucket[i];
        while(node)
        {
            UMMAPNODE* next = node->next;
            free(node);
            node = next;
        }
    }
    free(self->bucket);
    *self = JOIN(UMMAP, init)(self->hash, self->equal);
}

static inline void
JOIN(UMMAP, free)(UMMAP* self)
{
    JOIN(UMMAP, clear)(self);
    free(self->bucket);
    *self = JOIN(UMMAP, init)(self->hash, self->equal);
}

static inline UMMAP
JOIN(UMMAP, shallow_copy)(UMMAP* self)
{
    UMMAP other = JOIN(UMMAP, init)(self->hash, self->equal);

    other.copy_key = self->copy_key;
    other.copy_value = self->copy_value;
    other.free_key = NULL;
    other.free_value = NULL;

    UMMAPIT it = JOIN(UMMAPIT, each)(self);
    while(!it.done) {

        JOIN(UMMAP, insert)(&other, *it.key_ref, *it.value_ref);
        it.step(&it);
    }
    return other;
}

static inline UMMAP
JOIN(UMMAP, deep_copy)(UMMAP* self)
{
    UMMAP other = JOIN(UMMAP, init)(self->hash, self->equal);

    other.copy_key = self->copy_key;
    other.copy_value = self->copy_value;
    other.free_key = self->free_key;
    other.free_value = self->free_value;

    UMMAPIT it = JOIN(UMMAPIT, each)(self);
    while(!it.done) {
        K key_copy = other.copy_key ? other.copy_key(it.key_ref) : *it.key_ref;
        V value_copy = other.copy_value ? other.copy_value(it.value_ref) : *it.value_ref;
        JOIN(UMMAP, insert)(&other, key_copy, value_copy);
        it.step(&it);
    }
    return other;
}

static inline void
JOIN(UMMAP, swap)(UMMAP* self, UMMAP* other)
{
    UMMAP temp = *self;
    *self = *other;
    *other = temp;
}

static inline size_t
JOIN(UMMAP, size)(UMMAP* self)
{
    return self->size;
}

static inline size_t
JOIN(UMMAP, remove_if)(UMMAP* self, int _match(K*))
{
    size_t removed = 0;
    for(size_t i = 0; i < self->bucket_count; i++)
    {
        UMMAPNODE** bucket = &self->bucket[i];
        UMMAPNODE* prev = NULL;
        UMMAPNODE* node = *bucket;
        while(node)
        {
            UMMAPNODE* next = node->next;
            if(_match(&node->key))
            {
                if(self->free_key) self->free_key(&node->key);
                if(self->free_value) self->free_value(&node->value);

                if(prev)
                    prev->next = next;
                else
                    *bucket = next;
                free(node);
                self->size--;
                removed++;
                node = next;
            }
            else
            {
                prev = node;
                node = next;
            }
        }
    }
    return removed;
}

static inline int
JOIN(UMMAP, equal)(UMMAP* self, UMMAP* other)
{
    if(self->size != other->size)
        return 0;

    UMMAPIT it = JOIN(UMMAPIT, each)(self);
    while(!it.done) {
        if(!JOIN(UMMAP, find)(other, *it.key_ref))
            return 0;
        it.step(&it);
    }
    return 1;
}

static inline size_t
JOIN(UMMAP, bucket_count)(UMMAP* self)
{
    return self->bucket_count;
}

static inline size_t
JOIN(UMMAP, bucket_size)(UMMAP* self, size_t index)
{
    size_t size = 0;
    if(index < self->bucket_count) {
        for(UMMAPNODE* node = self->bucket[index]; node; node = node->next)
            size++;
    }
    return size;
}

static inline float
JOIN(UMMAP, load_factor)(UMMAP* self)
{
    return self->bucket_count > 0 ? (float)self->size / (float)self->bucket_count : 0.0f;
}

static inline void
JOIN(UMMAP, rehash)(UMMAP* self, size_t count)
{
    JOIN(UMMAP, reserve)(self, count);
}

#undef K
#undef V
#undef UMMAP
#undef UMMAPNODE
#undef UMMAPIT
