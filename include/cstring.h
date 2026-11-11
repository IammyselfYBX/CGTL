#ifndef __CGTL_STRING_H__
#define __CGTL_STRING_H__

#include "cgtl.h"
#include "iterator.h"
#include <stddef.h>
#include <string.h>

#define STRING_NPOS ((size_t)-1)
#define STRING_DEFAULT_CAPACITY 16

 typedef struct string {
    char* data;
    size_t size;
    size_t capacity;
} string;

static inline void string_ensure_capacity(string* str, size_t min_capacity) {
    if (min_capacity <= str->capacity) {
        return;
    }

    size_t new_capacity = str->capacity == 0 ? STRING_DEFAULT_CAPACITY : str->capacity;
    while (new_capacity <= min_capacity) {
        new_capacity *= 2;
    }

    char* new_data = (char*)realloc(str->data, new_capacity + 1);
    if (!new_data) {
        return;
    }

    str->data = new_data;
    str->capacity = new_capacity;
}

 static inline string string_new(void) {
    string str = {0};
    str.data = (char*)malloc(STRING_DEFAULT_CAPACITY + 1);
    if (str.data) {
        str.data[0] = '\0';
        str.capacity = STRING_DEFAULT_CAPACITY;
    }
    return str;
}

static inline string string_from_cstr(const char* cstr) {
    string str = {0};
    if (!cstr) {
        return string_new();
    }

    size_t len = strlen(cstr);
    string_ensure_capacity(&str, len);

    if (str.data) {
        strcpy(str.data, cstr);
        str.size = len;
    }
    return str;
}

static inline string string_from_char(char c, size_t count) {
    string str = {0};
    string_ensure_capacity(&str, count);

    if (str.data) {
        memset(str.data, c, count);
        str.data[count] = '\0';
        str.size = count;
    }
    return str;
}

static inline string string_copy(const string* src) {
    if (!src || !src->data) {
        return string_new();
    }
    return string_from_cstr(src->data);
}

static inline void string_free(string* str) {
    if (str && str->data) {
        free(str->data);
        str->data = NULL;
        str->size = 0;
        str->capacity = 0;
    }
}

 static inline size_t string_size(const string* str) {
    return str ? str->size : 0;
}

static inline size_t string_length(const string* str) {
    return string_size(str);
}

static inline int string_empty(const string* str) {
    return string_size(str) == 0;
}

static inline size_t string_capacity(const string* str) {
    return str ? str->capacity : 0;
}

static inline void string_reserve(string* str, size_t new_cap) {
    if (!str) return;
    string_ensure_capacity(str, new_cap);
}

static inline void string_shrink_to_fit(string* str) {
    if (!str || !str->data) return;

    if (str->size == 0) {

        str->capacity = STRING_DEFAULT_CAPACITY;
        str->data = (char*)realloc(str->data, str->capacity + 1);
        return;
    }

    if (str->capacity > str->size) {
        str->capacity = str->size;
        str->data = (char*)realloc(str->data, str->capacity + 1);

    }
}

 static inline char string_at_index(const string* str, size_t index) {
    if (!str || !str->data || index >= str->size) {
        return '\0';
    }
    return str->data[index];
}

static inline char string_at(const string* str, size_t index) {
    return string_at_index(str, index);
}

static inline char string_front(const string* str) {
    return string_at_index(str, 0);
}

static inline char string_back(const string* str) {
    if (!str || str->size == 0) {
        return '\0';
    }
    return string_at_index(str, str->size - 1);
}

static inline const char* string_data(const string* str) {
    return str && str->data ? str->data : "";
}

static inline const char* string_c_str(const string* str) {
    return string_data(str);
}

 static inline void string_clear(string* str) {
    if (!str || !str->data) return;
    str->data[0] = '\0';
    str->size = 0;
}

static inline void string_assign_cstr(string* str, const char* cstr) {
    if (!str) return;
    if (!cstr) {
        string_clear(str);
        return;
    }
    size_t len = strlen(cstr);
    string_ensure_capacity(str, len);
    if (str->data) {
        strcpy(str->data, cstr);
        str->size = len;
    }
}

static inline void string_assign_string(string* str, const string* other) {
    if (!str || !other) return;
    string_assign_cstr(str, other->data);
}

static inline void string_append_cstr(string* str, const char* cstr) {
    if (!str || !cstr) return;
    size_t cstr_len = strlen(cstr);
    if (cstr_len == 0) return;
    string_ensure_capacity(str, str->size + cstr_len);
    if (str->data) {
        strcat(str->data, cstr);
        str->size += cstr_len;
    }
}

static inline void string_append_string(string* str, const string* other) {
    if (!str || !other) return;
    string_append_cstr(str, other->data);
}

static inline void string_append_char(string* str, char c) {
    if (!str) return;
    string_ensure_capacity(str, str->size + 1);
    if (str->data) {
        str->data[str->size] = c;
        str->data[str->size + 1] = '\0';
        str->size++;
    }
}

static inline void string_plus_assign_cstr(string* str, const char* cstr) {
    string_append_cstr(str, cstr);
}

static inline void string_plus_assign_string(string* str, const string* other) {
    string_append_string(str, other);
}

static inline void string_plus_assign_char(string* str, char c) {
    string_append_char(str, c);
}

static inline void string_insert_cstr(string* str, size_t pos, const char* cstr) {
    if (!str || !cstr || pos > str->size) return;
    size_t cstr_len = strlen(cstr);
    if (cstr_len == 0) return;
    string_ensure_capacity(str, str->size + cstr_len);
    if (str->data) {
        memmove(str->data + pos + cstr_len, str->data + pos, str->size - pos + 1);
        memcpy(str->data + pos, cstr, cstr_len);
        str->size += cstr_len;
    }
}

static inline void string_insert_string(string* str, size_t pos, const string* other) {
    if (!str || !other) return;
    string_insert_cstr(str, pos, other->data);
}

static inline void string_insert_char(string* str, size_t pos, char c) {
    char temp[2] = {c, '\0'};
    string_insert_cstr(str, pos, temp);
}

static inline void string_erase(string* str, size_t pos, size_t len) {
    if (!str || !str->data || pos >= str->size) return;
    if (len == STRING_NPOS || pos + len >= str->size) {
        str->data[pos] = '\0';
        str->size = pos;
    } else {
        memmove(str->data + pos, str->data + pos + len, str->size - pos - len + 1);
        str->size -= len;
    }
}

static inline void string_replace_cstr(string* str, size_t pos, size_t len, const char* cstr) {
    if (!str || !cstr || pos > str->size) return;
    string_erase(str, pos, len);
    string_insert_cstr(str, pos, cstr);
}

static inline void string_replace_string(string* str, size_t pos, size_t len, const string* other) {
    if (!str || !other) return;
    string_replace_cstr(str, pos, len, other->data);
}

 static inline size_t string_find_cstr(const string* str, const char* substr) {
    if (!str || !str->data || !substr) return STRING_NPOS;
    const char* found = strstr(str->data, substr);
    return found ? (size_t)(found - str->data) : STRING_NPOS;
}

static inline size_t string_find_string(const string* str, const string* substr) {
    if (!str || !substr) return STRING_NPOS;
    return string_find_cstr(str, substr->data);
}

static inline size_t string_find_char(const string* str, char c) {
    if (!str || !str->data) return STRING_NPOS;
    const char* found = strchr(str->data, c);
    return found ? (size_t)(found - str->data) : STRING_NPOS;
}

static inline size_t string_rfind_cstr(const string* str, const char* substr) {
    if (!str || !str->data || !substr) return STRING_NPOS;
    size_t substr_len = strlen(substr);
    if (substr_len == 0) return str->size;
    if (substr_len > str->size) return STRING_NPOS;
    for (size_t i = str->size - substr_len; i != SIZE_MAX; i--) {
        if (strncmp(str->data + i, substr, substr_len) == 0) {
            return i;
        }
    }
    return STRING_NPOS;
}

static inline size_t string_rfind_string(const string* str, const string* substr) {
    if (!str || !substr) return STRING_NPOS;
    return string_rfind_cstr(str, substr->data);
}

static inline size_t string_rfind_char(const string* str, char c) {
    if (!str || !str->data) return STRING_NPOS;
    for (size_t i = str->size; i > 0; i--) {
        if (str->data[i-1] == c) {
            return i-1;
        }
    }
    return STRING_NPOS;
}

static inline string string_substr(const string* str, size_t pos, size_t len) {
    if (!str || pos >= str->size) {
        return string_new();
    }
    size_t actual_len = (len == STRING_NPOS || pos + len > str->size) ? str->size - pos : len;
    string result = string_new();
    string_ensure_capacity(&result, actual_len);
    if (result.data && actual_len > 0) {
        memcpy(result.data, str->data + pos, actual_len);
        result.data[actual_len] = '\0';
        result.size = actual_len;
    }
    return result;
}

static inline int string_compare_cstr(const string* str, const char* cstr) {
    if (!str || !str->data) return cstr ? -1 : 0;
    if (!cstr) return 1;
    return strcmp(str->data, cstr);
}

static inline int string_compare_string(const string* str, const string* other) {
    if (!str || !other) {
        return (!str && !other) ? 0 : (str ? 1 : -1);
    }
    return string_compare_cstr(str, other->data);
}

 static inline string string_concat_cstr(const string* str, const char* cstr) {
    string result = string_copy(str);
    string_append_cstr(&result, cstr);
    return result;
}

static inline string string_concat_string(const string* str1, const string* str2) {
    string result = string_copy(str1);
    string_append_string(&result, str2);
    return result;
}

static inline int string_equals_cstr(const string* str, const char* cstr) {
    return string_compare_cstr(str, cstr) == 0;
}

static inline int string_equals_string(const string* str1, const string* str2) {
    return string_compare_string(str1, str2) == 0;
}

static inline int string_less_than_cstr(const string* str, const char* cstr) {
    return string_compare_cstr(str, cstr) < 0;
}

static inline int string_less_than_string(const string* str1, const string* str2) {
    return string_compare_string(str1, str2) < 0;
}

static inline void
string_iter_step(cgtl_iter* it)
{
    if(it->done)
        return;

    char* cur = (char*)it->cur;
    char* end = (char*)it->end_ref;

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
string_iter_equal(const cgtl_iter* a, const cgtl_iter* b)
{
    if(a->done || b->done)
        return a->done && b->done;
    return a->cur == b->cur;
}

static inline ptrdiff_t
string_iter_distance(const cgtl_iter* a, const cgtl_iter* b)
{
    if(!a->cur || !b->cur)
        return 0;
    return (char*)b->cur - (char*)a->cur;
}

static inline cgtl_iter
string_iter_at(const cgtl_iter* a, ptrdiff_t n)
{
    cgtl_iter out = *a;
    char* cur = (char*)a->cur + n;
    out.cur = cur;
    out.ref = cur;
    out.done = (cur >= (char*)a->end_ref);
    return out;
}

static const cgtl_iter_ops string_iter_ops = {
    string_iter_step,
    string_iter_equal,
    string_iter_distance,
    string_iter_at
};

static inline cgtl_iter
string_iter_end(string* self)
{
    cgtl_iter it = {0};
    it.done = 1;
    it.container = self;
    it.cur = self->data + self->size;
    it.end_ref = it.cur;
    it.category = CGTL_ITER_RANDOM_ACCESS;
    it.ops = &string_iter_ops;
    return it;
}

static inline cgtl_iter
string_iter_begin(string* self)
{
    if(self->size == 0)
        return string_iter_end(self);

    cgtl_iter it = {0};
    it.ref = self->data;
    it.cur = self->data;
    it.end_ref = self->data + self->size;
    it.container = self;
    it.category = CGTL_ITER_RANDOM_ACCESS;
    it.ops = &string_iter_ops;
    return it;
}

static inline cgtl_range
string_iter_range(string* self)
{
    cgtl_range range;
    range.first = string_iter_begin(self);
    range.last = string_iter_end(self);
    return range;
}

#endif
