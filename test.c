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
#include "vector.h"

// Sanity check
TEST(test_construct_destruct,
{
  struct Vector vector;
  Vector_construct(&vector);
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
  struct Vector vector;
  Vector_destruct(&vector);
  ASSERT(vector.storage == NULL);
  ASSERT(vector.size == 0);
  ASSERT(vector.capacity == 0);
})

TEST(test_reserve,
{
  struct Vector vector;
  Vector_construct(&vector);

  Vector_reserve(&vector, 5);
  ASSERT(vector.capacity == 5);
  ASSERT(vector.storage != NULL);
  ASSERT(vector.size == 0);

  Vector_destruct(&vector);
})

TEST(test_resize,
{
  struct Vector vector;
  Vector_construct(&vector);

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
  struct Vector vector;
  Vector_construct(&vector);

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
  struct Vector vector;
  Vector_construct(&vector);

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
  struct Vector vector;
  Vector_construct(&vector);

  ASSERT(Vector_empty(&vector));

  Vector_resize(&vector, 3);
  ASSERT(!Vector_empty(&vector));

  Vector_resize(&vector, 0);
  ASSERT(Vector_empty(&vector));

  Vector_destruct(&vector);
})

int main()
{
  int failures = 0;

  failures += test_construct_destruct();
  failures += test_destruct_NULL();
  failures += test_reserve();
  failures += test_resize();
  failures += test_at();
  failures += test_size();
  failures += test_empty();

  if (failures)
  {
    printf("FAILED %d test cases\n", failures);
  }
  else
  {
    printf("OK\n");
  }
}
