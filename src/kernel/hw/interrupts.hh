#ifndef KERNEL_HW_INTERRUPTS_HH
#define KERNEL_HW_INTERRUPTS_HH

namespace kernel {
  namespace hw {
    namespace interrupts {

      void init();

      void enable();
      void disable();
    }
  }
}

#endif // KERNEL_HW_INTERRUPTS_HH
