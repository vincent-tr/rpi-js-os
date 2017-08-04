#ifndef KERNEL_HW_MEMORY_VM_REGION_HH
#define KERNEL_HW_MEMORY_VM_REGION_HH

namespace kernel {
  namespace hw {
    namespace memory {

      class vm_region {
        uint32_t addr;
        uint32_t len;
        vm_protection prot;
        const char *_name;

      public:
        explicit inline vm_region(const uint32_t &vaddr, const uint32_t &plen, const vm_protection &pprot, const char *pname)
         : addr(vaddr)
         , len(plen)
         , prot(pprot)
         , _name(pname) {
        }

        inline uint32_t address() const {
          return addr;
        }

        inline uint32_t address_end() const {
          return addr + len;
        }

        inline uint32_t length() const {
          return len;
        }

        inline vm_protection protection() const {
          return prot;
        }

        inline const char * name() const {
          return _name;
        }

        static void init();

        static vm_region *find(const uint32_t &address);
        static vm_region *create(const uint32_t &len, const vm_protection &prot, const char *name, const bool &is_internal = false);
        static void release(vm_region *region);
      };
    }
  }
}

#endif // KERNEL_HW_MEMORY_VM_REGION_HH
