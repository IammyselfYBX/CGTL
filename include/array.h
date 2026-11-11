#ifndef T
#error "Template type T undefined for <array.h>"
#endif

#ifndef N
#error "Array size N undefined for <array.h>"
#endif

#include "cgtl.h"
#include "iterator.h"

#define ARRAY JOIN(JOIN(array, T), N)
#define ARRAYIT JOIN(ARRAY, it)
#define CGTL_ALGO_PREFIX JOIN(ARRAY, algo)
#include "algorithm.h"
#undef CGTL_ALGO_PREFIX

typedef struct ARRAY {
    T data[N];
    T (*copy)(T*);
    void (*free)(T*);
} ARRAY;

typedef struct ARRAYIT {
    T* ref;
    T* next;
    T* begin;
    T* end;
    int done;
    void (*step)(struct ARRAYIT*);
} ARRAYIT;

static inline size_t
JOIN(ARRAY, size)(ARRAY* self)
{
    (void)self;
    return N;
}

static inline size_t
JOIN(ARRAY, max_size)(ARRAY* self)
{
    return JOIN(ARRAY, size)(self);
}

static inline int
JOIN(ARRAY, empty)(ARRAY* self)
{
    (void)self;
    return N == 0;
}

static inline T*
JOIN(ARRAY, begin)(ARRAY* self)
{
    return &self->data[0];
}

static inline T*
JOIN(ARRAY, end)(ARRAY* self)
{
    return &self->data[N];
}

static inline T*
JOIN(ARRAY, data)(ARRAY* self)
{
    return self->data;
}

static inline T*
JOIN(ARRAY, at)(ARRAY* self, size_t index)
{
    if (index >= N)
        return NULL;
    return &self->data[index];
}

static inline T*
JOIN(ARRAY, front)(ARRAY* self)
{
    return N > 0 ? &self->data[0] : NULL;
}

static inline T*
JOIN(ARRAY, back)(ARRAY* self)
{
    return N > 0 ? &self->data[N-1] : NULL;
}

static inline void
JOIN(ARRAY, set)(ARRAY* self, size_t index, T value)
{
    if (index < N)
    {
        if (self->free)
            self->free(&self->data[index]);

        if(self->copy) {

            self->data[index] = self->copy(&value);
        } else {

            self->data[index] = value;
        }
    }

}

static inline void
JOIN(ARRAYIT, step)(ARRAYIT* self)
{
    if(self->next == self->end)
        self->done = 1;
    else
    {
        self->ref = self->next;
        self->next++;
    }
}

static inline ARRAYIT
JOIN(ARRAYIT, range)(ARRAY* container, T* begin, T* end)
{
    (void)container;
    static ARRAYIT zero;
    ARRAYIT self = zero;
    if(begin && begin < end)
    {
        self.step = JOIN(ARRAYIT, step);
        self.ref = begin;
        self.next = begin + 1;
        self.begin = begin;
        self.end = end;
    }
    else
        self.done = 1;
    return self;
}

static inline ARRAYIT
JOIN(ARRAYIT, each)(ARRAY* self)
{
    return JOIN(ARRAY, empty)(self)
         ? JOIN(ARRAYIT, range)(self, NULL, NULL)
         : JOIN(ARRAYIT, range)(self, JOIN(ARRAY, begin)(self), JOIN(ARRAY, end)(self));
}

static inline void
JOIN(ARRAYIT, base_step)(cgtl_iter* it)
{
    if(it->done)
        return;

    T* cur = (T*)it->cur;
    T* end = (T*)it->end_ref;

    if(cur == end)
    {
        it->done = 1;
        return;
    }

    cur++;
    it->cur = cur;
    it->ref = cur;
    if(cur == end)
        it->done = 1;
}

static inline int
JOIN(ARRAYIT, base_equal)(const cgtl_iter* a, const cgtl_iter* b)
{
    if(a->done || b->done)
        return a->done && b->done;
    return a->cur == b->cur;
}

static inline ptrdiff_t
JOIN(ARRAYIT, base_distance)(const cgtl_iter* a, const cgtl_iter* b)
{
    if(!a->cur || !b->cur)
        return 0;
    return (T*)b->cur - (T*)a->cur;
}

static inline cgtl_iter
JOIN(ARRAYIT, base_at)(const cgtl_iter* a, ptrdiff_t n)
{
    cgtl_iter out = *a;
    T* cur = (T*)a->cur + n;
    out.cur = cur;
    out.ref = cur;
    out.done = (cur >= (T*)a->end_ref);
    return out;
}

static const cgtl_iter_ops JOIN(ARRAYIT, base_ops) = {
    JOIN(ARRAYIT, base_step),
    JOIN(ARRAYIT, base_equal),
    JOIN(ARRAYIT, base_distance),
    JOIN(ARRAYIT, base_at)
};

static inline cgtl_iter
JOIN(ARRAY, iter_end)(ARRAY* self)
{
    cgtl_iter it = {0};
    it.done = 1;
    it.container = self;
    it.cur = (N == 0) ? NULL : (void*)JOIN(ARRAY, end)(self);
    it.end_ref = it.cur;
    it.category = CGTL_ITER_RANDOM_ACCESS;
    it.ops = &JOIN(ARRAYIT, base_ops);
    return it;
}

static inline cgtl_iter
JOIN(ARRAY, iter_begin)(ARRAY* self)
{
    if(N == 0)
        return JOIN(ARRAY, iter_end)(self);

    cgtl_iter it = {0};
    it.ref = JOIN(ARRAY, begin)(self);
    it.cur = it.ref;
    it.end_ref = JOIN(ARRAY, end)(self);
    it.container = self;
    it.category = CGTL_ITER_RANDOM_ACCESS;
    it.ops = &JOIN(ARRAYIT, base_ops);
    return it;
}

static inline cgtl_range
JOIN(ARRAY, iter_range)(ARRAY* self)
{
    cgtl_range range;
    range.first = JOIN(ARRAY, iter_begin)(self);
    range.last = JOIN(ARRAY, iter_end)(self);
    return range;
}

static inline ARRAY
JOIN(ARRAY, init)(void)
{
    static ARRAY zero;
    ARRAY self = zero;

    self.copy = NULL;
    self.free = NULL;
    return self;
}

static inline void
JOIN(ARRAY, fill)(ARRAY* self, T value)
{
    for(size_t i = 0; i < N; i++)
    {
        if (self->free)
            self->free(&self->data[i]);

        if(self->copy) {

            self->data[i] = self->copy(&value);
        } else {

            self->data[i] = value;
        }
    }

}

static inline void
JOIN(ARRAY, swap)(ARRAY* self, ARRAY* other)
{
    for(size_t i = 0; i < N; i++)
    {
        T temp = self->data[i];
        self->data[i] = other->data[i];
        other->data[i] = temp;
    }
}

static inline void
JOIN(ARRAY, free)(ARRAY* self)
{
    if (self->free)
    {
        for(size_t i = 0; i < N; i++)
            self->free(&self->data[i]);
    }
    *self = JOIN(ARRAY, init)();
}

static inline void
JOIN(ARRAY, shallow_free)(ARRAY* self)
{

    *self = JOIN(ARRAY, init)();
}

static inline int
JOIN(ARRAY, equal)(ARRAY* self, ARRAY* other, int _equal(T*, T*))
{
    return JOIN(JOIN(ARRAY, algo), equal)(
        JOIN(ARRAY, iter_range)(self),
        JOIN(ARRAY, iter_range)(other),
        _equal);
}

static inline T*
JOIN(ARRAY, find)(ARRAY* self, T value, int _equal(T*, T*))
{
    cgtl_iter it = JOIN(JOIN(ARRAY, algo), find)(
        JOIN(ARRAY, iter_range)(self), value, _equal);
    return cgtl_iter_is_end(&it) ? NULL : (T*)it.ref;
}

static inline size_t
JOIN(ARRAY, count)(ARRAY* self, T value, int _equal(T*, T*))
{
    return JOIN(JOIN(ARRAY, algo), count)(
        JOIN(ARRAY, iter_range)(self), value, _equal);
}

static inline ARRAY
JOIN(ARRAY, shallow_copy)(ARRAY* self)
{
    ARRAY other = JOIN(ARRAY, init)();

    other.copy = self->copy;
    other.free = NULL;

    for(size_t i = 0; i < N; i++) {
        other.data[i] = self->data[i];
    }
    return other;
}

static inline ARRAY
JOIN(ARRAY, deep_copy)(ARRAY* self)
{
    ARRAY other = JOIN(ARRAY, init)();

    other.copy = self->copy;
    other.free = self->free;

    for(size_t i = 0; i < N; i++) {
        if(other.copy) {

            other.data[i] = other.copy(&self->data[i]);
        } else {

            other.data[i] = self->data[i];
        }
    }
    return other;
}

static inline ARRAY
JOIN(ARRAY, copy)(ARRAY* self)
{
    return JOIN(ARRAY, deep_copy)(self);
}

static inline size_t
JOIN(ARRAY, remove_if)(ARRAY* self, int _match(T*), T default_value)
{
    size_t removed = 0;
    cgtl_range range = JOIN(ARRAY, iter_range)(self);
    cgtl_iter cursor = range.first;

    while(!cgtl_iter_is_end(&cursor))
    {
        cgtl_range sub = { cursor, range.last };
        cgtl_iter found = JOIN(JOIN(ARRAY, algo), find_if)(sub, _match);
        if(cgtl_iter_is_end(&found))
            break;

        T* value = (T*)found.ref;
        if(self->free)
            self->free(value);
        if(self->copy)
            *value = self->copy(&default_value);
        else
            *value = default_value;
        removed++;

        cursor = found;
        cgtl_iter_step(&cursor);
    }
    return removed;
}

#undef T
#undef N
#undef ARRAY
#undef ARRAYIT
