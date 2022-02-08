#ifndef KIVANOV_C_TESTING
#define KIVANOV_C_TESTING

#include <stdio.h>

/* * USE THESE: * *
* TEST(name, body)
* ASSERT(statement)
* LOG_VECTOR(vec)
* LOG_ELEMENTS_INT(vec)
* ...
*/


/* Testing internals */

#ifdef TERM_COLORS
  #define _PASS_STRING "\033[32mPASS\033[0m"
  #define _FAIL_STRING "\033[31mFAIL\033[0m"
#else
  #define _PASS_STRING "PASS"
  #define _FAIL_STRING "FAIL"
#endif /* TERM_COLORS */

#define _LOG_TEST_START \
  printf("%s >\n", __func__)
#define _LOG_TEST_END(res) \
  printf("%s < %s\n", __func__, (res) ? _FAIL_STRING : _PASS_STRING)


/* For logging of Vector internals */
// TODO: Separate these off
#ifndef QUIET
  #define LOG_VECTOR(vec) \
    printf("  %s: size=%zu, capacity=%zu, storage=%p\n", \
          #vec, vec.size, vec.capacity, vec.storage)

  #define LOG_ELEMENTS_INT(vec) \
    for (size_t i = 0; i < vec.size; i++) \
      printf("  %s[%zu]=(%d)", #vec, i, vec.storage[i]); \
    putchar('\n');
#else
  #define LOG_VECTOR(vec)
  #define LOG_ELEMENTS_INT(vec)
#endif /* QUIET */


/* General testing utilities */
// TODO: SetUp and TearDown
#define TEST(name, body) \
static void name() { int _RESULT = 0; _LOG_TEST_START; { body } _LOG_TEST_END(_RESULT); }

#define ASSERT(statement) \
  if (!(statement)) { \
    printf("  " _FAIL_STRING " Assertion (%s) failed.\n", #statement); \
    _RESULT = 1; \
  }

#endif // KIVANOV_C_TESTING