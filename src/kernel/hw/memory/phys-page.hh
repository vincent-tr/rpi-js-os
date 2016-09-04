#ifndef KERNEL_HW_MEMORY_HH
#define KERNEL_HW_MEMORY_HH

namespace kernel {
  namespace hw {
    namespace memory {

      class phys_page {
        uint32_t address;

      public:
        explicit inline phys_page(const uint32_t &paddr)
         : address(paddr) {
        }

        explicit inline operator uint32_t() const {
          return address;
        }

        static phys_page alloc();
        void free();

        static void init(uint32_t &desc_begin, uint32_t &desc_end);
      };

    }
  }
}

#endif // KERNEL_HW_MEMORY_HH
