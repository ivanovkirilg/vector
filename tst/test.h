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

#ifndef QUIET
  #define _LOG_TEST_START \
    printf("%s >\n", __func__)
  #define _LOG_TEST_END(res) \
    printf("%s < %s\n", __func__, (res) ? _FAIL_STRING : _PASS_STRING)
#else
  #define _LOG_TEST_START
  #define _LOG_TEST_END(res) \
    printf("%s %s\n", __func__, (res) ? _FAIL_STRING : _PASS_STRING)
#endif


/* General testing utilities */
#define TEST(name, body)                                                       \
  static int name() {                                                          \
    int _RESULT = 0;                                                           \
    _LOG_TEST_START;                                                           \
    {body} _LOG_TEST_END(_RESULT);                                             \
    return _RESULT;                                                            \
  }

#define ASSERT(statement)                                                      \
  if (!(statement)) {                                                          \
    printf("  " _FAIL_STRING " Assertion (%s) failed.\n", #statement);         \
    _RESULT = 1;                                                               \
  }

#define TEST_MAIN(testcases, ...)                                              \
  typedef int (*testcase_t)();                                                 \
  testcase_t _testcases[] = { testcases, ##__VA_ARGS__ };                      \
  int main(void) {                                                             \
    int failures = 0;                                                          \
    for (int i = 0; i < sizeof(_testcases) / sizeof(_testcases[0]); i++)       \
      failures += _testcases[i]();                                             \
                                                                               \
    if (failures)                                                              \
      printf(_FAIL_STRING ": %d\n", failures);                                 \
    else                                                                       \
      printf(_PASS_STRING "\n");                                               \
    return failures;                                                           \
  }

#endif // KIVANOV_C_TESTING