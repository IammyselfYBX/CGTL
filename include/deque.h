#ifndef T
#error "Template type T undefined for <deque.h>"
#endif
#include "cgtl.h"

#define DEQ JOIN(deq, T)
#define DEQBUCKET JOIN(DEQ, bucket)
#define DEQIT JOIN(DEQ, it)

#define DEQ_BUCKET_SIZE (512)

typedef struct DEQBUCKET
{
    T value[DEQ_BUCKET_SIZE];
    int16_t a;
    int16_t b;
} DEQBUCKET;

typedef struct DEQ
{
    DEQBUCKET** pages;
    size_t mark_a;
    size_t mark_b;
    size_t capacity;
    size_t size;
    T (*copy)(T*);
    void (*free)(T*);
} DEQ;

typedef struct DEQIT
{
    DEQ* container;
    T* ref;
    size_t index;
    size_t index_next;
    size_t index_last;
    int done;
    void (*step)(struct DEQIT*);
} DEQIT;

static inline DEQBUCKET**
JOIN(DEQ, first)(DEQ* self)
{
    return &self->pages[self->mark_a];
}

static inline DEQBUCKET**
JOIN(DEQ, last)(DEQ* self)
{
    return &self->pages[self->mark_b - 1];
}

static inline T*
JOIN(DEQ, at)(DEQ* self, size_t index)
{

    if(self->size == 0 || index >= self->size){
        return NULL;
    }

    DEQBUCKET* first_bucket = self->pages[self->mark_a];

    size_t absolute_index = index + first_bucket->a;

    size_t bucket_offset = absolute_index / DEQ_BUCKET_SIZE;

    size_t element_offset = absolute_index % DEQ_BUCKET_SIZE;

    DEQBUCKET* target_bucket = self->pages[self->mark_a + bucket_offset];

    return &target_bucket->value[element_offset];
}

static inline T*
JOIN(DEQ, front)(DEQ* self)
{
    return JOIN(DEQ, at)(self, 0);
}

static inline T*
JOIN(DEQ, back)(DEQ* self)
{
    return JOIN(DEQ, at)(self, self->size - 1);
}

static inline T*
JOIN(DEQ, begin)(DEQ* self)
{
    return JOIN(DEQ, front)(self);
}

static inline T*
JOIN(DEQ, end)(DEQ* self)
{
    return JOIN(DEQ, back)(self) + 1;
}

static inline void
JOIN(DEQIT, step)(DEQIT* self)
{
    self->index = self->index_next;
    if(self->index == self->index_last)
        self->done = 1;
    else
    {
        self->ref = JOIN(DEQ, at)(self->container, self->index);
        self->index_next += 1;
    }
}

static inline DEQIT
JOIN(DEQIT, range)(DEQ* container, T* begin, T* end)
{
    static DEQIT zero;
    DEQIT self = zero;
    if(begin && end)
    {
        self.container = container;
        self.step = JOIN(DEQIT, step);
        self.index = begin - JOIN(DEQ, begin)(container);
        self.index_next = self.index + 1;
        self.index_last = container->size - (JOIN(DEQ, end)(container) - end);
        self.ref = JOIN(DEQ, at)(container, self.index);
    }
    else
        self.done = 1;
    return self;
}

static inline int
JOIN(DEQ, empty)(DEQ* self)
{
    return self->size == 0;
}

static inline DEQIT
JOIN(DEQIT, each)(DEQ* a)
{
    return JOIN(DEQ, empty)(a)
         ? JOIN(DEQIT, range)(a, NULL, NULL)
         : JOIN(DEQIT, range)(a, JOIN(DEQ, begin)(a), JOIN(DEQ, end)(a));
}

static inline int
JOIN(DEQ, equal)(DEQ* self, DEQ* other, int _equal(T*, T*))
{
    if(self->size != other->size)
        return 0;
    DEQIT a = JOIN(DEQIT, each)(self);
    DEQIT b = JOIN(DEQIT, each)(other);
    while(!a.done && !b.done)
    {
        if(!_equal(a.ref, b.ref))
            return 0;
        a.step(&a);
        b.step(&b);
    }
    return 1;
}

static inline void
JOIN(DEQ, swap)(DEQ* self, DEQ* other)
{
    DEQ temp = *self;
    *self = *other;
    *other = temp;
}

static inline DEQ
JOIN(DEQ, init)(void)
{
    static DEQ zero;
    DEQ self = zero;

    self.copy = NULL;
    self.free = NULL;
    return self;
}

static inline DEQBUCKET*
JOIN(DEQBUCKET, init)(size_t cut)
{

    DEQBUCKET* self = (DEQBUCKET*) calloc(1, sizeof(DEQBUCKET));
    self->a = self->b = cut;
    return self;
}

static inline void
JOIN(DEQ, set)(DEQ* self, size_t index, T value)
{
    T* ref = JOIN(DEQ, at)(self, index);
    if(self->free)
        self->free(ref);
    *ref = value;
}

static inline void
JOIN(DEQ, alloc)(DEQ* self, size_t capacity, size_t shift_from)
{
    self->capacity = capacity;
    self->pages = (DEQBUCKET**) realloc(self->pages, capacity * sizeof(DEQBUCKET*));
    size_t shift = (self->capacity - shift_from) / 2;
    size_t i = self->mark_b;
    while(i != 0)
    {
        i -= 1;
        self->pages[i + shift] = self->pages[i];
    }
    self->mark_a += shift;
    self->mark_b += shift;
}

static inline void
JOIN(DEQ, push_front)(DEQ* self, T value)
{
    if(JOIN(DEQ, empty)(self))
    {
        self->mark_a = 0;
        self->mark_b = 1;
        JOIN(DEQ, alloc)(self, 1, 0);
        *JOIN(DEQ, last)(self) = JOIN(DEQBUCKET, init)(DEQ_BUCKET_SIZE);
    }
    else
    {
        DEQBUCKET* page = *JOIN(DEQ, first)(self);
        if(page->a == 0)
        {
            if(self->mark_a == 0)
                JOIN(DEQ, alloc)(self, 2 * self->capacity, self->mark_a);
            self->mark_a -= 1;
            *JOIN(DEQ, first)(self) = JOIN(DEQBUCKET, init)(DEQ_BUCKET_SIZE);
        }
    }
    DEQBUCKET* page = *JOIN(DEQ, first)(self);
    page->a -= 1;
    self->size += 1;
    page->value[page->a] = value;
}

static inline void
JOIN(DEQ, pop_front)(DEQ* self)
{
    DEQBUCKET* page = *JOIN(DEQ, first)(self);
    if(self->free)
    {
        T* ref = &page->value[page->a];
        self->free(ref);
    }
    page->a += 1;
    self->size -= 1;
    if(page->a == page->b)
    {
        free(page);
        self->mark_a += 1;
    }
}

static inline void
JOIN(DEQ, push_back)(DEQ* self, T value)
{

    if(JOIN(DEQ, empty)(self))
    {
        self->mark_a = 0;
        self->mark_b = 1;
        JOIN(DEQ, alloc)(self, 1, 0);
        *JOIN(DEQ, last)(self) = JOIN(DEQBUCKET, init)(0);
    }
    else
    {
        DEQBUCKET* page = *JOIN(DEQ, last)(self);
        if(page->b == DEQ_BUCKET_SIZE)
        {
            if(self->mark_b == self->capacity)
                JOIN(DEQ, alloc)(self, 2 * self->capacity, self->mark_b);
            self->mark_b += 1;
            *JOIN(DEQ, last)(self) = JOIN(DEQBUCKET, init)(0);
        }
    }
    DEQBUCKET* page = *JOIN(DEQ, last)(self);
    page->value[page->b] = value;
    page->b += 1;
    self->size += 1;
}

static inline void
JOIN(DEQ, pop_back)(DEQ* self)
{
    DEQBUCKET* page = *JOIN(DEQ, last)(self);
    page->b -= 1;
    self->size -= 1;
    if(self->free)
    {
        T* ref = &page->value[page->b];
        self->free(ref);
    }
    if(page->b == page->a)
    {
        free(page);
        self->mark_b -= 1;
    }
}

static inline void
JOIN(DEQ, erase)(DEQ* self, size_t index)
{
    static T zero;
    JOIN(DEQ, set)(self, index, zero);
    void (*saved)(T*) = self->free;
    self->free = NULL;
    if(index < self->size / 2)
    {
        for(size_t i = index; i > 0; i--)
            *JOIN(DEQ, at)(self, i) = *JOIN(DEQ, at)(self, i - 1);
        JOIN(DEQ, pop_front)(self);
    }
    else
    {
        for(size_t i = index; i < self->size - 1; i++)
            *JOIN(DEQ, at)(self, i) = *JOIN(DEQ, at)(self, i + 1);
        JOIN(DEQ, pop_back)(self);
    }
    self->free = saved;
}

static inline void
JOIN(DEQ, insert)(DEQ* self, size_t index, T value)
{
    if(self->size > 0)
    {
        void (*saved)(T*) = self->free;
        self->free = NULL;
        if(index < self->size / 2)
        {
            JOIN(DEQ, push_front)(self, *JOIN(DEQ, at)(self, 0));
            for(size_t i = 0; i < index; i++)
                *JOIN(DEQ, at)(self, i) = *JOIN(DEQ, at)(self, i + 1);
        }
        else
        {
            JOIN(DEQ, push_back)(self, *JOIN(DEQ, at)(self, self->size - 1));
            for(size_t i = self->size - 1; i > index; i--)
                *JOIN(DEQ, at)(self, i) = *JOIN(DEQ, at)(self, i - 1);
        }
        *JOIN(DEQ, at)(self, index) = value;
        self->free = saved;
    }
    else
        JOIN(DEQ, push_back)(self, value);
}

static inline void
JOIN(DEQ, resize)(DEQ* self, size_t size, T value)
{
    if(size < self->size){
        while(size < self->size)
            JOIN(DEQ, pop_back)(self);
    }else{

        while(self->size < size) {
            if(self->copy) {

                JOIN(DEQ, push_back)(self, self->copy(&value));
            } else {

                JOIN(DEQ, push_back)(self, value);
            }
        }
    }

}

static inline void
JOIN(DEQ, clear)(DEQ* self)
{
    while(!JOIN(DEQ, empty)(self))
        JOIN(DEQ, pop_back)(self);
}

static inline void
JOIN(DEQ, assign)(DEQ* self, size_t size, T value)
{
    JOIN(DEQ, clear)(self);

    for(size_t i = 0; i < size; i++) {
        if(self->copy) {

            JOIN(DEQ, push_back)(self, self->copy(&value));
        } else {

            JOIN(DEQ, push_back)(self, value);
        }
    }

}

static inline void
JOIN(DEQ, shallow_free)(DEQ* self)
{

    for(size_t i = self->mark_a; i < self->mark_b; i++)
        free(self->pages[i]);
    free(self->pages);
    *self = JOIN(DEQ, init)();
}

static inline void
JOIN(DEQ, free)(DEQ* self)
{
    JOIN(DEQ, clear)(self);
    free(self->pages);
    *self = JOIN(DEQ, init)();
}

static inline DEQ
JOIN(DEQ, shallow_copy)(DEQ* self)
{
    DEQ other = JOIN(DEQ, init)();

    other.copy = self->copy;
    other.free = NULL;

    while(other.size < self->size)
    {
        T* value = JOIN(DEQ, at)(self, other.size);
        JOIN(DEQ, push_back)(&other, *value);
    }
    return other;
}

static inline DEQ
JOIN(DEQ, deep_copy)(DEQ* self)
{
    DEQ other = JOIN(DEQ, init)();

    other.copy = self->copy;
    other.free = self->free;

    if (other.copy) {
        while(other.size < self->size)
        {
            T* value = JOIN(DEQ, at)(self, other.size);
            JOIN(DEQ, push_back)(&other, other.copy(value));
        }
    } else {
        while(other.size < self->size)
        {
            T* value = JOIN(DEQ, at)(self, other.size);
            JOIN(DEQ, push_back)(&other, *value);
        }
    }
    return other;
}

static inline void
JOIN(DEQ, ranged_sort)(DEQ* self, size_t a, size_t b, int _compare(T*, T*))
{

    if(a >= b) return;

    size_t mid = a + (b - a) / 2;
    SWAP(T, JOIN(DEQ, at)(self, a), JOIN(DEQ, at)(self, mid));

    size_t z = a;

    for (size_t i = a + 1; i <= b; i++) {

        if (_compare(JOIN(DEQ, at)(self, i), JOIN(DEQ, at)(self, a)) < 0) {
            z++;
            SWAP(T, JOIN(DEQ, at)(self, z), JOIN(DEQ, at)(self, i));
        }
    }

    SWAP(T, JOIN(DEQ, at)(self, a), JOIN(DEQ, at)(self, z));

    if (z > a) {
        JOIN(DEQ, ranged_sort)(self, a, z - 1, _compare);
    }

    JOIN(DEQ, ranged_sort)(self, z + 1, b, _compare);
}

static inline void
JOIN(DEQ, sort)(DEQ* self, int _compare(T*, T*))
{
    JOIN(DEQ, ranged_sort)(self, 0, self->size - 1, _compare);
}

static inline size_t
JOIN(DEQ, remove_if)(DEQ* self, int _match(T*))
{
    size_t erases = 0;
    foreach(DEQ, self, it)
        if(_match(it.ref))
        {
            JOIN(DEQ, erase)(self, it.index);
            it.index_next = it.index;
            it.index_last -= 1;
            erases += 1;
        }
    return erases;
}

static inline T*
JOIN(DEQ, find)(DEQ* self, T key, int _equal(T*, T*))
{
    foreach(DEQ, self, it)
        if(_equal(it.ref, &key))
            return it.ref;
    return NULL;
}

#undef T
#undef DEQ
#undef DEQBUCKET
#undef DEQIT

#undef DEQ_BUCKET_SIZE
