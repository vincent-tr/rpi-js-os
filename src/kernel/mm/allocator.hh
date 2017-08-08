#ifndef KERNEL_MM_ALLOCATOR_HH
#define KERNEL_MM_ALLOCATOR_HH

namespace kernel {
  namespace mm {

    struct allocator {
      static void init();

      static void *allocate(const uint32_t &size);
      static void deallocate(void *ptr);
    };

  }
}

#endif // KERNEL_MM_ALLOCATOR_HH
