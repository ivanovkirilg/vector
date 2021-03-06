/* WARNING:
   This header purposefully  HAS  NO  HEADER GUARD.
   This was done to enable definition of multiple vector types.
*/

#include <stdlib.h>

/* INTERFACE */
// TODO: Raise error instead!
#ifndef VECTOR_TYPENAME
  #define VECTOR_TYPENAME Vector
#endif
#ifndef VECTOR_ELEMENT_TYPE
  #define VECTOR_ELEMENT_TYPE int
#endif
/* !INTERFACE */

/* Error codes */
#define VECTOR_NO_ERR 0
#define VECTOR_ALLOC_ERR 1

/* Macro magic to figure out the actual function names */
#define ___VECTOR_FUNC(_prefix, function) \
  _prefix ## _ ## function
#define __VECTOR_FUNC(_prefix, function) \
  ___VECTOR_FUNC(_prefix, function)
#define _VECTOR_FUNC(function) \
  __VECTOR_FUNC(VECTOR_TYPENAME, function)

struct VECTOR_TYPENAME {
  size_t size;
  size_t capacity;
  VECTOR_ELEMENT_TYPE *storage;
};

// Equivalent to initializing with { 0 }
static void _VECTOR_FUNC(construct)(struct VECTOR_TYPENAME *self)
{
  self->size = 0;
  self->capacity = 0;
  self->storage = NULL;
}

static void _VECTOR_FUNC(destruct)(struct VECTOR_TYPENAME *self)
{
  if (self->storage && self->capacity)
  {
    free(self->storage);
  }
  else if (self->storage || self->capacity)
  {
    // TODO LOG destroying uninitialized vector
  }

  self->storage = NULL;
  self->size = 0;
  self->capacity = 0;
}

static int _VECTOR_FUNC(reserve)(struct VECTOR_TYPENAME *self, size_t n)
{
  if (self->capacity < n)
  {
    self->storage = (VECTOR_ELEMENT_TYPE *)
      realloc( self->storage, n * sizeof(VECTOR_ELEMENT_TYPE) );
  }

  // TODO LOG alloc error
  return (self->storage == NULL) ? VECTOR_ALLOC_ERR : VECTOR_NO_ERR; // DEP@resize
}

static int _VECTOR_FUNC(resize)(struct VECTOR_TYPENAME *self, size_t n)
{
  // Growing
  if (n > self->size)
  {
    if (_VECTOR_FUNC(reserve)(self, n))
    {
      return VECTOR_ALLOC_ERR;
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
  return VECTOR_NO_ERR;
}

#undef VECTOR_TYPENAME
#undef VECTOR_ELEMENT_TYPE
#undef VECTOR_ELEMENT_CONSTRUCTOR
#undef VECTOR_ELEMENT_DESTRUCTOR