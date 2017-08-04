#ifndef KERNEL_HW_MEMORY_VM_PROTECTION_HH
#define KERNEL_HW_MEMORY_VM_PROTECTION_HH

namespace kernel {
  namespace hw {
    namespace memory {

      struct vm_protection {
        bool read: 1;
        bool write: 1;
        //bool execute: 1;
      };
    }
  }
}

#endif // KERNEL_HW_MEMORY_VM_PROTECTION_HH
