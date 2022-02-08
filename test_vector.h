#ifndef KIVANOV_C_TESTING_VECTOR
#define KIVANOV_C_TESTING_VECTOR

#include "test.h"

#define LOG_VECTOR(vec) \
  LOG("%s: size=%zu, capacity=%zu, storage=%p", \
      #vec, vec.size, vec.capacity, vec.storage)

#define LOG_ELEMENTS_INT(vec) \
  do { \
      for (size_t i = 0; i < vec.size; i++) \
      LOG("%s[%zu]=(%d)", #vec, i, vec.storage[i]); \
  } while (0)

#endif // KIVANOV_C_TESTING_VECTOR