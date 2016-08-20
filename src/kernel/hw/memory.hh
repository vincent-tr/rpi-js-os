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
        void free();
      };

      void init(const uint32_t &page_size, const uint32_t &ram_size);
    }
  }
}

#endif // KERNEL_HW_MEMORY_HH
