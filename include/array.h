#ifndef T
#error "Template type T undefined for <array.h>"
#endif

#ifndef N
#error "Array size N undefined for <array.h>"
#endif

#include "cgtl.h"

#define ARRAY JOIN(JOIN(array, T), N)
#define ARRAYIT JOIN(ARRAY, it)

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
    for(size_t i = 0; i < N; i++)
    {
        if(!_equal(&self->data[i], &other->data[i]))
            return 0;
    }
    return 1;
}

static inline T*
JOIN(ARRAY, find)(ARRAY* self, T value, int _equal(T*, T*))
{
    for(size_t i = 0; i < N; i++)
    {
        if(_equal(&self->data[i], &value))
            return &self->data[i];
    }
    return NULL;
}

static inline size_t
JOIN(ARRAY, count)(ARRAY* self, T value, int _equal(T*, T*))
{
    size_t count = 0;
    for(size_t i = 0; i < N; i++)
    {
        if(_equal(&self->data[i], &value))
            count++;
    }
    return count;
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
    for(size_t i = 0; i < N; i++)
    {
        if(_match(&self->data[i]))
        {
            if (self->free)
                self->free(&self->data[i]);

            if(self->copy) {

                self->data[i] = self->copy(&default_value);
            } else {

                self->data[i] = default_value;
            }
            removed++;
        }
    }

    return removed;
}

#undef T
#undef N
#undef ARRAY
#undef ARRAYIT
