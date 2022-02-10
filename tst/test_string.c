#include "test_vector.h"

#include <stdlib.h>

static void construct(char ** el)
{
  *el = calloc(25, 1);
}
static void destruct(char **el)
{
  free(*el);
  *el = NULL;
}

#define VECTOR_TYPENAME StrVec
#define VECTOR_ELEMENT_TYPE char *
#define VECTOR_ELEMENT_CONSTRUCTOR construct
#define VECTOR_ELEMENT_DESTRUCTOR destruct
#include "../vector.h"

// Sanity check
TEST(test_construct_destruct,
{
  struct StrVec vector;
  StrVec_construct(&vector);
  ASSERT(vector.storage == NULL);
  ASSERT(vector.size == 0);
  ASSERT(vector.capacity == 0);
  StrVec_destruct(&vector);
  ASSERT(vector.storage == NULL);
  ASSERT(vector.size == 0);
  ASSERT(vector.capacity == 0);
})

TEST(test_destruct_NULL,
{
  struct StrVec vector;
  StrVec_destruct(&vector);
  ASSERT(vector.storage == NULL);
  ASSERT(vector.size == 0);
  ASSERT(vector.capacity == 0);
})

TEST(test_resize,
{
  struct StrVec vector;
  StrVec_construct(&vector);

  StrVec_resize(&vector, 5);
  for (size_t i = 0; i < 5; i++)
  {
    for (size_t j = 0; j < 25; j++)
    {
      // Our constructor was called for each element
      ASSERT(vector.storage[i][j] == 0);
    }
  }

  StrVec_destruct(&vector);
})


int main()
{
  int failures = 0;

  failures += test_construct_destruct();
  failures += test_destruct_NULL();
  failures += test_resize();

  if (failures)
  {
    printf("FAILED %d test cases\n", failures);
  }
  else
  {
    printf("OK\n");
  }
}
