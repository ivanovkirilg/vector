#ifndef KIVANOV_C_VECTOR
#define KIVANOV_C_VECTOR

#include <stdlib.h>
#include <stdbool.h>


/* * * INTERFACE * * */
#ifndef VECTOR_TYPENAME
  #define VECTOR_TYPENAME Vector
#endif
#ifndef VECTOR_ELEMENT_TYPE
  #define VECTOR_ELEMENT_TYPE int
#endif


/* * * Error codes * * */
#define VECTOR_ERROR_NONE 0
#define VECTOR_ERROR_NULL 1
#define VECTOR_ERROR_INVALID 2
#define VECTOR_ERROR_ALLOC 3


/* * *  Internals * * */
struct VECTOR_TYPENAME {
  size_t size;
  size_t capacity;
  VECTOR_ELEMENT_TYPE *storage;
};

#define _VECTOR_BASE_CAPACITY 8u
#define _VECTOR_CAPACITY_GROWTH_RATE 1.6

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

#define _VECTOR_NULL_RETURN(vec, ret_val) \
  do { \
    if (vec == NULL) { \
      _VECTOR_LOG_ERROR( #vec " is NULL."); \
      return ret_val; \
    } \
  } while (0)

static int __VECTOR_VERIFY_INVARIANTS(
  const struct VECTOR_TYPENAME *vec, const char *func)
{
  _VECTOR_NULL_RETURN(vec, VECTOR_ERROR_NULL);

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

// Precondition: invariants already verified
static int __VECTOR_GROW_CAPACITY(
  struct VECTOR_TYPENAME *vec, const char *func)
{
  if (vec->capacity < _VECTOR_BASE_CAPACITY)
  {
    vec->capacity = _VECTOR_BASE_CAPACITY;
  }
  else
  {
    vec->capacity *= _VECTOR_CAPACITY_GROWTH_RATE;
  }

  vec->storage = (VECTOR_ELEMENT_TYPE *)
    realloc( vec->storage, vec->capacity * sizeof(VECTOR_ELEMENT_TYPE) );

  if (vec->storage == NULL)
  {
    __VECTOR_LOG_ERROR(func, "Allocation for capacity growth failed.");
    *vec = (struct VECTOR_TYPENAME){ 0 };
    return VECTOR_ERROR_ALLOC; // DEP@push_back
  }
  return VECTOR_ERROR_NONE;
}
#define _VECTOR_GROW_CAPACITY(vec) \
  __VECTOR_GROW_CAPACITY(vec, __func__)



/* * * Functions * * */

static void _VECTOR_FUNC(destruct)(struct VECTOR_TYPENAME *self)
{
  _VECTOR_NULL_RETURN(self, );

  if ((_VECTOR_VERIFY_INVARIANTS(self) == VECTOR_ERROR_NONE)
      && self->storage)
  {
#ifdef VECTOR_ELEMENT_DESTRUCTOR
    for (size_t i = 0; i < self->size; i++)
    {
      VECTOR_ELEMENT_DESTRUCTOR(&self->storage[i]);
    }
#endif
    free(self->storage);
  }

  *self = (struct VECTOR_TYPENAME){ 0 };
}

static int _VECTOR_FUNC(reserve)(struct VECTOR_TYPENAME *self, size_t n)
{
  _VECTOR_NULL_RETURN(self, VECTOR_ERROR_NULL);

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
  _VECTOR_NULL_RETURN(self, VECTOR_ERROR_NULL);

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
  _VECTOR_NULL_RETURN(self, NULL);

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
  _VECTOR_NULL_RETURN(self, 0);

  if (_VECTOR_VERIFY_INVARIANTS(self) != VECTOR_ERROR_NONE)
  {
    _VECTOR_LOG_ERROR("Invalid representation, "
                      "returning size=0 instead of %zu.", self->size);
    return 0;
  }

  return self->size;
}

static bool _VECTOR_FUNC(empty)(const struct VECTOR_TYPENAME *self)
{
  _VECTOR_NULL_RETURN(self, true);

  if (_VECTOR_VERIFY_INVARIANTS(self) != VECTOR_ERROR_NONE)
  {
    _VECTOR_LOG_ERROR("Invalid representation, returning empty=true.");
    return true;
  }

  return (self->size == 0);
}

// A 'copy constructor' can be provided via VECTOR_ELEMENT_COPY.
// Otherwise, the element is passed by value (default),
// or if VECTOR_REF_COPY is defined, by address.
static int _VECTOR_FUNC(push_back)(
#ifdef VECTOR_REF_COPY
  struct VECTOR_TYPENAME *self, const VECTOR_ELEMENT_TYPE *val)
#else
  struct VECTOR_TYPENAME *self, const VECTOR_ELEMENT_TYPE val)
#endif // VECTOR_REF_COPY
{
  _VECTOR_NULL_RETURN(self, VECTOR_ERROR_NULL);

  if (_VECTOR_VERIFY_INVARIANTS(self) != VECTOR_ERROR_NONE)
  {
    return VECTOR_ERROR_INVALID;
  }

  self->size++;
  if (self->size >= self->capacity) // > should be impossible
  {
    if (_VECTOR_GROW_CAPACITY(self) != VECTOR_ERROR_NONE)
    {
      return VECTOR_ERROR_ALLOC;
    }
  }
#ifdef VECTOR_ELEMENT_COPY
  VECTOR_ELEMENT_COPY(val, &self->storage[self->size-1]);
#else

  #ifdef VECTOR_REF_COPY
  // Cast to explicitly discard const
  self->storage[self->size-1] = (VECTOR_ELEMENT_TYPE) *val;
  #else
  self->storage[self->size-1] = (VECTOR_ELEMENT_TYPE) val;
  #endif // VECTOR_REF_COPY
#endif // VECTOR_ELEMENT_COPY

  return VECTOR_ERROR_NONE;
}

#undef VECTOR_TYPENAME
#undef VECTOR_ELEMENT_TYPE
#undef VECTOR_ELEMENT_COPY
#undef VECTOR_REF_COPY
#undef VECTOR_ELEMENT_CONSTRUCTOR
#undef VECTOR_ELEMENT_DESTRUCTOR

#endif // KIVANOV_C_VECTOR
