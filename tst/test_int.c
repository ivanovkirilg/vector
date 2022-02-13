#include "test_vector.h"

static const int DESTROYED = -9999;
static void construct(int *el)
{
  *el = 0;
}
static void destruct(int *el)
{
  *el = DESTROYED;
}

#define VECTOR_ELEMENT_CONSTRUCTOR construct
#define VECTOR_ELEMENT_DESTRUCTOR destruct
#include "../vector.h"

// Sanity check
TEST(test_construct_destruct,
{
  struct Vector vector = {0};
  ASSERT(vector.storage == NULL);
  ASSERT(vector.size == 0);
  ASSERT(vector.capacity == 0);
  Vector_destruct(&vector);
  ASSERT(vector.storage == NULL);
  ASSERT(vector.size == 0);
  ASSERT(vector.capacity == 0);
})

TEST(test_destruct_NULL,
{
  struct Vector vector = {0};
  Vector_destruct(&vector);
  ASSERT(vector.storage == NULL);
  ASSERT(vector.size == 0);
  ASSERT(vector.capacity == 0);
})

TEST(test_reserve,
{
  struct Vector vector = {0};

  Vector_reserve(&vector, 5);
  ASSERT(vector.capacity == 5);
  ASSERT(vector.storage != NULL);
  ASSERT(vector.size == 0);

  Vector_destruct(&vector);
})

TEST(test_resize,
{
  struct Vector vector = {0};

  // Reserve
  Vector_resize(&vector, 3);
  ASSERT(vector.storage != NULL);
  ASSERT(vector.size == 3);
  ASSERT(vector.size <= vector.capacity);

  // Shrink and call destructor
  Vector_resize(&vector, 2);
  ASSERT(vector.size == 2);
  ASSERT(vector.size <= vector.capacity);
  ASSERT(vector.storage[2] == DESTROYED);

  // Grow and call constructor
  Vector_resize(&vector, 4);
  ASSERT(vector.size == 4);
  ASSERT(vector.size <= vector.capacity);
  ASSERT(vector.storage[2] == 0);
  ASSERT(vector.storage[3] == 0);

  Vector_destruct(&vector);
})

TEST(test_at,
{
  struct Vector vector = {0};

  ASSERT(Vector_at(&vector, 1) == NULL);

  Vector_resize(&vector, 3);
  ASSERT(Vector_at(&vector, -1) == NULL);
  ASSERT(Vector_at(&vector, 3) == NULL);
  ASSERT(Vector_at(&vector, 1) != NULL);
  ASSERT(Vector_at(&vector, 0) == vector.storage);
  ASSERT(Vector_at(&vector, 1) > vector.storage);
  // LOG_VECTOR(vector);
  // LOG_ELEMENTS_INT(vector);

  Vector_destruct(&vector);
})

TEST(test_size,
{
  struct Vector vector = {0};

  ASSERT(Vector_size(&vector) == 0);

  Vector_resize(&vector, 5);
  ASSERT(Vector_size(&vector) == 5);
  // Invalidate the vector
  vector.size = 99;
  ASSERT(Vector_size(&vector) == 0);

  // Revert
  vector.size = 5;
  Vector_destruct(&vector);
})

TEST(test_empty,
{
  struct Vector vector = {0};

  ASSERT(Vector_empty(&vector));

  Vector_resize(&vector, 3);
  ASSERT(!Vector_empty(&vector));

  Vector_resize(&vector, 0);
  ASSERT(Vector_empty(&vector));

  Vector_destruct(&vector);
})

TEST(test_push_back,
{
  struct Vector vector = {0};

  Vector_push_back(&vector, 5);
  ASSERT(vector.capacity == _VECTOR_BASE_CAPACITY);
  ASSERT(vector.size == 1u);
  ASSERT(vector.storage[0] == 5);

  Vector_resize(&vector, _VECTOR_BASE_CAPACITY);
  Vector_push_back(&vector, -42);
  ASSERT(vector.capacity ==
         (size_t) (_VECTOR_BASE_CAPACITY * _VECTOR_CAPACITY_GROWTH_RATE));
  ASSERT(vector.size == 9u);
  ASSERT(vector.storage[0] == 5);
  ASSERT(vector.storage[8] == -42);

  // LOG_VECTOR(vector);
  // LOG_ELEMENTS_INT(vector);
  Vector_destruct(&vector);
})

TEST(test_NULL,
{
  // Basically, don't crash
  Vector_destruct(NULL);

  ASSERT(Vector_reserve(NULL, 5) == VECTOR_ERROR_NULL);
  ASSERT(Vector_resize(NULL, 5) == VECTOR_ERROR_NULL);
  ASSERT(Vector_at(NULL, 5) == NULL);
  ASSERT(Vector_size(NULL) == 0);
  ASSERT(Vector_empty(NULL) == true);
  ASSERT(Vector_push_back(NULL, 0) == VECTOR_ERROR_NULL);
})


TEST_MAIN(
  test_construct_destruct,
  test_destruct_NULL,
  test_reserve,
  test_resize,
  test_at,
  test_size,
  test_empty,
  test_push_back,
  test_NULL
)
