#ifndef TEST_TEST_HH
#define TEST_TEST_HH

#ifdef QEMU
#define TEST_ENABLED
#endif

namespace test {

#ifdef TEST_ENABLED

  void utils_list();
  void hw_memory();

#else // TEST_ENABLED

  inline void utils_list() { }
  inline void hw_memory() { }

#endif // TEST_ENABLED

}

#endif // TEST_TEST_HH
