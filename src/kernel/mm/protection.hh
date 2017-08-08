#ifndef KERNEL_MM_PROTECTION_HH
#define KERNEL_MM_PROTECTION_HH

namespace kernel {
  namespace mm {

    struct protection {
      bool read: 1;
      bool write: 1;
      //bool execute: 1;
    };
  }
}

#endif // KERNEL_MM_PROTECTION_HH
