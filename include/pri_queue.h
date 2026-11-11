#ifndef T
#error "Template type T undefined for <pri_queue.h>"
#endif

#define front         top
#define at            __AT
#define back          __BACK
#define begin         __BEGIN
#define end           __END
#define set           __SET
#define pop_back      __POP_BACK
#define wipe          __WIPE
#define clear         __CLEAR
#define fit           __FIT
#define reserve       __RESERVE
#define push_back     __PUSH_BACK
#define resize        __RESIZE
#define assign        __ASSIGN
#define shrink_to_fit __SHRINK_TO_FIT
#define data          __DATA
#define insert        __INSERT
#define erase         __ERASE
#define sort          __SORT
#define step          __STEP
#define range         __RANGE
#define each          __EACH
#define remove_if     __REMOVE_IF
#define shallow_copy  __SHALLOW_COPY
#define deep_copy     __DEEP_COPY
#define shallow_free  __SHALLOW_FREE

#define PRIORITY_QUEUE_CONTEXT
#define vec pqu
#define HOLD
#define COMPARE
#define init __INIT
#include <vector.h>
#undef init
#undef vec
#undef PRIORITY_QUEUE_CONTEXT

#define PRIQUE JOIN(pqu, T)

static inline PRIQUE
JOIN(PRIQUE, init)(int _compare(T*, T*))
{
    PRIQUE self = JOIN(PRIQUE, __INIT)();
    self.compare = _compare;

    self.copy = NULL;
    self.free = NULL;
    return self;
}

static inline void
JOIN(PRIQUE, up)(PRIQUE* self, size_t n)
{
    if(n > 0)
    {
        size_t p = (n - 1) / 2;

        T* x = &self->value[n];
        T* y = &self->value[p];
        if(self->compare(x, y)){
            SWAP(T, x, y);
            JOIN(PRIQUE, up)(self, p);
        }
    }
}

static inline void
JOIN(PRIQUE, down)(PRIQUE* self, size_t n)
{
    size_t min = 2;
    if(self->size < min){
        return;
    }else if(self->size == min){

        T* a = &self->value[0];
        T* b = &self->value[1];

        if (!self->compare(a, b))
            SWAP(T, a, b);
    }else{

        size_t l = 2 * n + 1;
        size_t r = 2 * n + 2;
        if(r < self->size){
            size_t index = self->compare(&self->value[r], &self->value[l]) ? r : l;
            T* x = &self->value[index];
            T* y = &self->value[n];
            if(self->compare(x, y)){
                SWAP(T, x, y);
                JOIN(PRIQUE, down)(self, index);
            }
        }
    }
}

static inline void
JOIN(PRIQUE, push)(PRIQUE* self, T value)
{
    JOIN(PRIQUE, push_back)(self, value);
    JOIN(PRIQUE, up)(self, self->size - 1);
}

static inline void
JOIN(PRIQUE, pop)(PRIQUE* self)
{
    SWAP(T, JOIN(PRIQUE, front)(self), JOIN(PRIQUE, back)(self));
    JOIN(PRIQUE, pop_back)(self);
    JOIN(PRIQUE, down)(self, 0);
}

#undef front
#undef at
#undef back
#undef begin
#undef end
#undef set
#undef pop_back
#undef wipe
#undef clear
#undef fit
#undef reserve
#undef push_back
#undef resize
#undef assign
#undef shrink_to_fit
#undef data
#undef insert
#undef erase
#undef sort
#undef step
#undef range
#undef each
#undef remove_if
#undef shallow_copy
#undef deep_copy
#undef shallow_free

#undef T
#undef PRIQUE
