#ifndef T
#error "Template type T undefined for <vector.h>"
#endif

#include "cgtl.h"
#define MUST_ALIGN_16(T) (sizeof(T) == sizeof(char))

#define VEC JOIN(vec, T)
typedef struct VEC
{
    T* value;
    size_t size;
    size_t capacity;
    T (*copy)(T*);
    void (*free)(T*);
#ifdef COMPARE
    int (*compare)(T*, T*);
#endif
} VEC;

#define VECIT JOIN(VEC, it)
typedef struct VECIT
{
    T*   ref;
    T*   next;
    T*   begin;
    T*   end;
    int done;
    void (*step)(struct VECIT*);
} VECIT;

static inline T*
JOIN(VEC, at)(VEC* self, size_t index)
{
    return &self->value[index];
}

static inline T*
JOIN(VEC, front)(VEC* self)
{
    return JOIN(VEC, at)(self, 0);
}

static inline T*
JOIN(VEC, back)(VEC* self)
{
    return JOIN(VEC, at)(self, self->size - 1);
}

static inline T*
JOIN(VEC, begin)(VEC* self)
{
    return JOIN(VEC, front)(self);
}

static inline T*
JOIN(VEC, end)(VEC* self)
{
    return JOIN(VEC, back)(self) + 1;
}

static inline void
JOIN(VECIT, step)(VECIT* self)
{
    if(self->next >= self->end){
        self->done = 1;
    }else{
        self->ref = self->next;
        self->next += 1;
    }
}

static inline VECIT
JOIN(VECIT, range)(VEC* container, T* begin, T* end)
{
    (void) container;
    static VECIT zero;
    VECIT self = zero;
    if(begin && end){
        self.step = JOIN(VECIT, step);
        self.begin = begin;
        self.end = end;
        self.next = begin + 1;
        self.ref = begin;
    }else{
        self.done = 1;
    }
    return self;
}

static inline int
JOIN(VEC, empty)(VEC* self)
{
    return self->size == 0;
}

static inline VECIT
JOIN(VECIT, each)(VEC* a)
{
    return JOIN(VEC, empty)(a)
         ? JOIN(VECIT, range)(a, NULL, NULL)
         : JOIN(VECIT, range)(a, JOIN(VEC, begin)(a), JOIN(VEC, end)(a));
}

static inline int
JOIN(VEC, equal)(VEC* self, VEC* other, int _equal(T*, T*))
{
    if(self->size != other->size){return 0;}
    VECIT a = JOIN(VECIT, each)(self);
    VECIT b = JOIN(VECIT, each)(other);
    while(!a.done && !b.done)
    {
        if(!_equal(a.ref, b.ref)){return 0;}
        a.step(&a);
        b.step(&b);
    }
    return 1;
}

static inline void
JOIN(VEC, swap)(VEC* self, VEC* other)
{
    VEC temp = *self;
    *self = *other;
    *other = temp;
}

static inline VEC
JOIN(VEC, init)(void)
{
    static VEC zero;
    VEC self = zero;

    self.copy = NULL;
    self.free = NULL;
    return self;
}

static inline void
JOIN(VEC, set)(VEC* self, size_t index, T value)
{
    T* ref = JOIN(VEC, at)(self, index);

    if(self->free)
        self->free(ref);
    *ref = value;
}

static inline void
JOIN(VEC, pop_back)(VEC* self)
{
    static T zero;
    self->size -= 1;
    JOIN(VEC, set)(self, self->size, zero);
}

static inline void
JOIN(VEC, wipe)(VEC* self, size_t n)
{
    while(n != 0)
    {
        JOIN(VEC, pop_back)(self);
        n -= 1;
    }
}

static inline void
JOIN(VEC, clear)(VEC* self)
{
    if(self->size > 0)
        JOIN(VEC, wipe)(self, self->size);
}

static inline void
JOIN(VEC, shallow_free)(VEC* self)
{

    free(self->value);
    *self = JOIN(VEC, init)();
}

static inline void
JOIN(VEC, free)(VEC* self)
{
    JOIN(VEC, clear)(self);
    free(self->value);
    *self = JOIN(VEC, init)();
}

static inline void
JOIN(VEC, fit)(VEC* self, size_t capacity)
{
    static T zero;
    size_t overall = capacity;
    if(MUST_ALIGN_16(T))
        overall += 1;
    self->value = (T*) realloc(self->value, overall * sizeof(T));
    if(MUST_ALIGN_16(T))
        for(size_t i = self->capacity; i < overall; i++)
            self->value[i] = zero;
    self->capacity = capacity;
}

static inline void
JOIN(VEC, reserve)(VEC* self, const size_t capacity)
{
    if(capacity != self->capacity){
        size_t actual = 0;
        if(MUST_ALIGN_16(T)){
            if(capacity <= self->size){actual = self->size;}
            else{
                if(capacity > self->size && capacity < self->capacity){
                    actual = capacity;
                }else{
                    actual = 2 * self->capacity;
                    if(capacity > actual)
                        actual = capacity;
                }
            }
        }else{
            if(capacity > self->capacity)
                actual = capacity;
            if(actual > 0)
                JOIN(VEC, fit)(self, actual);
        }
    }
}

static inline void
JOIN(VEC, push_back)(VEC* self, T value)
{
    if(self->size == self->capacity)
        JOIN(VEC, reserve)(self, self->capacity == 0 ? 1 : 2 * self->capacity);
    *JOIN(VEC, at)(self, self->size) = value;
    self->size += 1;
}

static inline void
JOIN(VEC, resize)(VEC* self, size_t size, T value)
{
    if(size < self->size){
        int64_t less = self->size - size;
        if(less > 0)
            JOIN(VEC, wipe)(self, less);
    }else{
        if(size > self->capacity){
            size_t capacity = 2 * self->size;
            if(size > capacity)
                capacity = size;
            JOIN(VEC, reserve)(self, capacity);
        }

        for(size_t i = 0; self->size < size; i++) {
            if(self->copy) {

                JOIN(VEC, push_back)(self, self->copy(&value));
            } else {

                JOIN(VEC, push_back)(self, value);
            }
        }
    }

}

static inline void
JOIN(VEC, assign)(VEC* self, size_t size, T value)
{
    JOIN(VEC, clear)(self);
    JOIN(VEC, reserve)(self, size);

    for(size_t i = 0; i < size; i++) {
        if(self->copy) {

            JOIN(VEC, push_back)(self, self->copy(&value));
        } else {

            JOIN(VEC, push_back)(self, value);
        }
    }

}

static inline void
JOIN(VEC, shrink_to_fit)(VEC* self)
{
    JOIN(VEC, fit)(self, self->size);
}

static inline T*
JOIN(VEC, data)(VEC* self)
{
    return JOIN(VEC, front)(self);
}

static inline void
JOIN(VEC, insert)(VEC* self, size_t index, T value)
{
    if(self->size > 0)
    {
        JOIN(VEC, push_back)(self, *JOIN(VEC, back)(self));
        for(size_t i = self->size - 2; i > index; i--)
            self->value[i] = self->value[i - 1];
        self->value[index] = value;
    }
    else{
        JOIN(VEC, push_back)(self, value);
    }
}

static inline void
JOIN(VEC, erase)(VEC* self, size_t index)
{
    static T zero;
    JOIN(VEC, set)(self, index, zero);
    for(size_t i = index; i < self->size - 1; i++)
    {
        self->value[i] = self->value[i + 1];
        self->value[i + 1] = zero;
    }
    self->size -= 1;
}

static inline void
JOIN(VEC, ranged_sort)(VEC* self, int64_t a, int64_t b, int _compare(T*, T*))
{

    if(a >= b || self->size == 0 || a < 0 || b >= (int64_t)self->size){
        return;
    }

    int64_t mid = a + (b - a) / 2;

    SWAP(T, &self->value[a], &self->value[mid]);

    int64_t z = a;

    for (int64_t i = a + 1; i <= b; i++) {

        if (_compare(&self->value[i], &self->value[a]) <= 0) {
            z++;
            SWAP(T, &self->value[z], &self->value[i]);
        }
    }

    SWAP(T, &self->value[a], &self->value[z]);

    JOIN(VEC, ranged_sort)(self, a, z - 1, _compare);

    JOIN(VEC, ranged_sort)(self, z + 1, b, _compare);
}

static inline void
JOIN(VEC, sort)(VEC* self, int _compare(T*, T*))
{

    if(self->size <= 1) {
        return;
    }

    JOIN(VEC, ranged_sort)(self, 0, (int64_t)(self->size - 1), _compare);
}

static inline size_t
JOIN(VEC, remove_if)(VEC* self, int _match(T*))
{
    size_t erases = 0;
    foreach(VEC, self, it){
        if(_match(it.ref)){
            size_t index = it.ref - JOIN(VEC, begin)(self);
            JOIN(VEC, erase)(self, index);
            it.end = JOIN(VEC, end)(self);
            it.next = it.ref;
            erases += 1;
        }
    }
    return erases;
}

static inline T*
JOIN(VEC, find)(VEC* self, T key, int _equal(T*, T*))
{
    foreach(VEC, self, it)
        if(_equal(it.ref, &key))
            return it.ref;
    return NULL;
}

static inline VEC
JOIN(VEC, shallow_copy)(VEC* self)
{
    VEC other = JOIN(VEC, init)();

    other.copy = self->copy;
    other.free = NULL;

#ifdef COMPARE
    other.compare = self->compare;
#endif
    JOIN(VEC, reserve)(&other, self->size);

    while(other.size < self->size)
        JOIN(VEC, push_back)(&other, self->value[other.size]);
    return other;
}

static inline VEC
JOIN(VEC, deep_copy)(VEC* self)
{
    VEC other = JOIN(VEC, init)();

    other.copy = self->copy;
    other.free = self->free;

#ifdef COMPARE
    other.compare = self->compare;
#endif
    JOIN(VEC, reserve)(&other, self->size);
    if (other.copy) {
        while(other.size < self->size)
            JOIN(VEC, push_back)(&other, other.copy(&self->value[other.size]));
    } else {
        while(other.size < self->size)
            JOIN(VEC, push_back)(&other, self->value[other.size]);
    }
    return other;
}

#ifdef COMPARE
    #undef COMPARE
#endif

#undef VEC
#undef VECIT
#undef MUST_ALIGN_16

#ifdef HOLD
#undef HOLD
#else
    #undef T
#endif
