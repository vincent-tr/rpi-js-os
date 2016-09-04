#ifndef KERNEL_HW_MMU_HH
#define KERNEL_HW_MMU_HH

namespace kernel {
  namespace hw {
    namespace memory {

      enum vm_protection {

      };

      class vm_page {
        uint32_t address;

      public:
        explicit inline page(const uint32_t &vaddr)
         : address(paddr) {
        }

        explicit inline operator uint32_t() const {
          return address;
        }

        uint32_t phys() const;
        vm_protection protection() const;

        void map();
        void unmap();

        static void init();
      };
    }
  }
}

#endif // KERNEL_HW_MMU_HH
