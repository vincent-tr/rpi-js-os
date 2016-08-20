#ifndef KERNEL_HW_MEMORY_HH
#define KERNEL_HW_MEMORY_HH

namespace kernel {
  namespace hw {
    namespace memory {

      class page {
        void *address;

      public:
        explicit inline page(void *paddr)
         : address(paddr) {
        }

        explicit inline operator void*() const {
          return address;
        }

        static page alloc();
        static void free(page &p);
      };

      void init();
    }
  }
}

#endif // KERNEL_HW_MEMORY_HH
