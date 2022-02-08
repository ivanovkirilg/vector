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

TEST(test_reserve,
{
  struct Vector vector;
  Vector_construct(&vector);

  Vector_reserve(&vector, 5);
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

  // Shrink and call destructor
  Vector_resize(&vector, 2);
  ASSERT(vector.size == 2);
  ASSERT(vector.storage[2] == DESTROYED);

  // Grow and call constructor
  Vector_resize(&vector, 4);
  ASSERT(vector.size == 4);
  ASSERT(vector.storage[2] == 0);
  ASSERT(vector.storage[3] == 0);

  Vector_destruct(&vector);
})

int main()
{
  test_construct_destruct();
  test_reserve();
  test_resize();
}
