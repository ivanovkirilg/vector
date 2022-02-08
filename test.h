#ifndef KIVANOV_C_TESTING
#define KIVANOV_C_TESTING

#include <stdio.h>

/* Logging */
#ifndef QUIET
  #define LOG(format, ...) \
    printf("  " format "\n", ##__VA_ARGS__ )
#else
  #define LOG(...)
#endif /* QUIET */


/* Internals */
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


/* General testing utilities */
#define TEST(name, body) \
static void name() { int _RESULT = 0; _LOG_TEST_START; { body } _LOG_TEST_END(_RESULT); }

#define ASSERT(statement) \
  if (!(statement)) { \
    printf("  " _FAIL_STRING " Assertion (%s) failed.\n", #statement); \
    _RESULT = 1; \
  }

#endif // KIVANOV_C_TESTING