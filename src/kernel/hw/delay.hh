#ifndef KERNEL_HW_DELAY_HH
#define KERNEL_HW_DELAY_HH

#include <stddef.h>
#include <stdint.h>

namespace kernel {
  namespace hw {

    /* Loop <delay> times in a way that the compiler won't optimize away. */
    static inline void delay(int32_t count) {
      asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
         : "=r"(count): [count]"0"(count) : "cc");
    }
  }
}

#endif // KERNEL_HW_DELAY_HH
