#ifndef KERNEL_HW_MMU_HH
#define KERNEL_HW_MMU_HH

namespace kernel {
  namespace hw {
    namespace memory {

      struct vm_protection {
        bool read: 1;
        bool write: 1;
      };

      class vm_page {
        uint32_t address;

      public:
        explicit inline vm_page(const uint32_t &vaddr)
         : address(vaddr) {
        }

        explicit inline operator uint32_t() const {
          return address;
        }

        phys_page phys() const;
        const vm_protection &protection() const;

        void map(const vm_protection &prot, const phys_page &phys);
        void unmap();

        static void init(uint32_t &desc_end);
      };
    }
  }
}

#endif // KERNEL_HW_MMU_HH
