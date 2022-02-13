#include "test_vector.h"

#include <string.h>
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
static void copy(const char *src, char **dst)
{
  *dst = strdup(src);
}

#define VECTOR_TYPENAME StrVec
#define VECTOR_ELEMENT_TYPE char *
#define VECTOR_ELEMENT_COPY copy
#define VECTOR_ELEMENT_CONSTRUCTOR construct
#define VECTOR_ELEMENT_DESTRUCTOR destruct
#include "../vector.h"

// Sanity check
TEST(test_generic,
{
  struct StrVec vector = {0};
  ASSERT(vector.storage == NULL);
  ASSERT(vector.size == 0);
  ASSERT(vector.capacity == 0);
  ASSERT(StrVec_empty(&vector) == true);

  StrVec_resize(&vector, 5);
  ASSERT(StrVec_empty(&vector) == false);
  ASSERT(StrVec_size(&vector) == 5);

  StrVec_destruct(&vector);
  ASSERT(vector.storage == NULL);
  ASSERT(vector.size == 0);
  ASSERT(vector.capacity == 0);
})

TEST(test_resize,
{
  struct StrVec vector = {0};

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

static int compare(const void *left, const void *right)
{
  return ((char**)left) [0][0] - ((char**)right) [0][0];
}

TEST(test_names,
{
  struct StrVec vector = {0};

  StrVec_resize(&vector, 1);

  strcpy(*StrVec_at(&vector, 0), "Bob");
  StrVec_push_back(&vector,  "Charlie Ferguson");
  StrVec_push_back(&vector, "Alice");
  LOG_ELEMENTS_STRING(vector);

  qsort(vector.storage, vector.size, sizeof(char*), compare);
  LOG_ELEMENTS_STRING(vector);

  ASSERT(strcmp(vector.storage[0], "Alice") == 0);
  ASSERT(strcmp(vector.storage[1], "Bob") == 0);
  ASSERT(strcmp(vector.storage[2], "Charlie Ferguson") == 0);

  StrVec_destruct(&vector);
})

TEST_MAIN(
  test_generic,
  test_resize,
  test_names
)
