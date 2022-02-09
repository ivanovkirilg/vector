#ifndef KIVANOV_C_VECTOR
#define KIVANOV_C_VECTOR

#include <stdlib.h>
#include <stdbool.h>

/* INTERFACE */
// TODO: Definition macro instead
#ifndef VECTOR_TYPENAME
  #define VECTOR_TYPENAME Vector
#endif
#ifndef VECTOR_ELEMENT_TYPE
  #define VECTOR_ELEMENT_TYPE int
#endif
/* !INTERFACE */

/* Error codes */
#define VECTOR_ERROR_NONE 0
#define VECTOR_ERROR_ALLOC 1
#define VECTOR_ERROR_INVALID 2

struct VECTOR_TYPENAME {
  size_t size;
  size_t capacity;
  VECTOR_ELEMENT_TYPE *storage;
};

#if defined(NDEBUG) || defined(VECTOR_QUIET)
  #define __VECTOR_LOG_ERROR(func, msg, ...)
  #define _VECTOR_LOG_ERROR(msg, ...)
#else
  #include <stdio.h>
  #define __VECTOR_LOG_ERROR(func, msg, ...) \
    fprintf(stderr, "%s: " msg "\n", func, ##__VA_ARGS__ )
  #define _VECTOR_LOG_ERROR(msg, ...) \
    __VECTOR_LOG_ERROR(__func__, msg, ##__VA_ARGS__ )
#endif

static int __VECTOR_VERIFY_INVARIANTS(
  const struct VECTOR_TYPENAME *vec, const char *func)
{
  if (vec->capacity == 0 && vec->storage != NULL)
  {
    __VECTOR_LOG_ERROR(func,
      "Invalid vector representation: capacity==0 but storage!=NULL.");
    return VECTOR_ERROR_INVALID;
  }
  if (vec->capacity > 0  && vec->storage == NULL)
  {
    __VECTOR_LOG_ERROR(func,
      "Invalid vector representation: storage==NULL but capacity!=0.");
    return VECTOR_ERROR_INVALID;
  }
  if (vec->capacity < vec->size)
  {
    __VECTOR_LOG_ERROR(func,
      "Invalid vector representation: capacity<size.");
    return VECTOR_ERROR_INVALID;
  }

  return VECTOR_ERROR_NONE;
}
#define _VECTOR_VERIFY_INVARIANTS(vec) \
  __VECTOR_VERIFY_INVARIANTS(vec, __func__)

/* Macro magic to figure out the actual function names */
#define ___VECTOR_FUNC(_prefix, function) \
  _prefix ## _ ## function
#define __VECTOR_FUNC(_prefix, function) \
  ___VECTOR_FUNC(_prefix, function)
#define _VECTOR_FUNC(function) \
  __VECTOR_FUNC(VECTOR_TYPENAME, function)

static void _VECTOR_FUNC(construct)(struct VECTOR_TYPENAME *self)
{
  *self = (struct VECTOR_TYPENAME){ 0 };
}

static void _VECTOR_FUNC(destruct)(struct VECTOR_TYPENAME *self)
{
  if ((_VECTOR_VERIFY_INVARIANTS(self) == VECTOR_ERROR_NONE)
      && self->storage)
  {
    free(self->storage);
  }

  *self = (struct VECTOR_TYPENAME){ 0 };
}

static int _VECTOR_FUNC(reserve)(struct VECTOR_TYPENAME *self, size_t n)
{
  if (_VECTOR_VERIFY_INVARIANTS(self) != VECTOR_ERROR_NONE)
  {
    *self = (struct VECTOR_TYPENAME){ 0 }; // DEP@resize
    return VECTOR_ERROR_INVALID;
  }

  if (self->capacity < n)
  {
    self->storage = (VECTOR_ELEMENT_TYPE *)
      realloc( self->storage, n * sizeof(VECTOR_ELEMENT_TYPE) );

    if (self->storage == NULL)
    {
      _VECTOR_LOG_ERROR("Allocation failed.");
      *self = (struct VECTOR_TYPENAME){ 0 }; // DEP@resize
      return VECTOR_ERROR_ALLOC;
    }
  }

  self->capacity = n;
  return VECTOR_ERROR_NONE;
}

static int _VECTOR_FUNC(resize)(struct VECTOR_TYPENAME *self, size_t n)
{
  if (_VECTOR_VERIFY_INVARIANTS(self) != VECTOR_ERROR_NONE)
  {
    *self = (struct VECTOR_TYPENAME){ 0 };
    return VECTOR_ERROR_INVALID;
  }
  // Growing
  if (n > self->size)
  {
    if (_VECTOR_FUNC(reserve)(self, n))
    {
      return VECTOR_ERROR_ALLOC;
    }
#ifdef VECTOR_ELEMENT_CONSTRUCTOR
    for (size_t i = self->size; i < n; i++)
    {
      VECTOR_ELEMENT_CONSTRUCTOR(&self->storage[i]);
    }
#endif
  }
  // Shrinking
#ifdef VECTOR_ELEMENT_DESTRUCTOR
  else if (n < self->size)
  {
    for (size_t i = n; i < self->size; i++)
    {
      VECTOR_ELEMENT_DESTRUCTOR(&self->storage[i]);
    }
  }
#endif

  self->size = n;
  return VECTOR_ERROR_NONE;
}

static VECTOR_ELEMENT_TYPE *_VECTOR_FUNC(at)(
  const struct VECTOR_TYPENAME *self, size_t n)
{
  if ((_VECTOR_VERIFY_INVARIANTS(self) != VECTOR_ERROR_NONE))
  {
    return NULL;
  }

  if (n < 0 || n >= self->size)
  {
    _VECTOR_LOG_ERROR("Index %zu out of bounds (size=%zu).", n, self->size);
    return NULL;
  }

  return &self->storage[n];
}

static size_t _VECTOR_FUNC(size)(const struct VECTOR_TYPENAME *self)
{
  if (_VECTOR_VERIFY_INVARIANTS(self) != VECTOR_ERROR_NONE)
  {
    _VECTOR_LOG_ERROR("Invalid representation, "
                      "returning size 0 instead of %zu.", self->size);
    return 0;
  }

  return self->size;
}

static bool _VECTOR_FUNC(empty)(const struct VECTOR_TYPENAME *self)
{
  return (self->size == 0);
}

#undef VECTOR_TYPENAME
#undef VECTOR_ELEMENT_TYPE
#undef VECTOR_ELEMENT_CONSTRUCTOR
#undef VECTOR_ELEMENT_DESTRUCTOR

#endif // KIVANOV_C_VECTOR
