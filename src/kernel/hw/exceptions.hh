#ifndef KERNEL_HW_EXCEPTIONS_HH
#define KERNEL_HW_EXCEPTIONS_HH

namespace kernel {
  namespace hw {
    namespace exceptions {

      void init();

      void enable();
      void disable();
    }
  }
}

#endif // KERNEL_HW_EXCEPTIONS_HH
