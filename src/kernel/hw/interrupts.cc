#include <stddef.h>
#include <stdint.h>

#include "interrupts.hh"
#include "kernel/utils/debug.hh"

// https://github.com/brianwiddas/pi-baremetal/blob/master/interrupts.c
// https://github.com/BrianSidebotham/arm-tutorial-rpi/blob/master/part-4/armc-013/rpi-interrupts.c

namespace kernel {
  namespace hw {
    namespace interrupts {

      /**
          @brief The Reset vector interrupt handler
          This can never be called, since an ARM core reset would also reset the
          GPU and therefore cause the GPU to start running code again until
          the ARM is handed control at the end of boot loading
      */
      extern "C" void __attribute__((naked, interrupt("ABORT"))) reset_vector() {
        DEBUG("reset_vector");
      }

      /**
          @brief The undefined instruction interrupt handler
          If an undefined intstruction is encountered, the CPU will start
          executing this function. Just trap here as a debug solution.
      */
      extern "C" void __attribute__((naked, interrupt("UNDEF"))) undefined_instruction_vector() {
        DEBUG("undefined_instruction_vector");
        for(;;);
      }

      /**
          @brief The supervisor call interrupt handler
          The CPU will start executing this function. Just trap here as a debug
          solution.
      */
      extern "C" void __attribute__((naked, interrupt("SWI"))) software_interrupt_vector() {
        DEBUG("software_interrupt_vector");
        for(;;);
      }

      /**
          @brief The prefetch abort interrupt handler
          The CPU will start executing this function. Just trap here as a debug
          solution.
      */
      extern "C" void __attribute__((naked, interrupt("ABORT"))) prefetch_abort_vector() {
        DEBUG("prefetch_abort_vector");
        for(;;);
      }

      /**
          @brief The Data Abort interrupt handler
          The CPU will start executing this function. Just trap here as a debug
          solution.
      */
      extern "C" void __attribute__((naked, interrupt("ABORT"))) data_abort_vector() {
        DEBUG("data_abort_vector");
        for(;;);
      }

      /**
          @brief The IRQ Interrupt handler
          This handler is run every time an interrupt source is triggered. It's
          up to the handler to determine the source of the interrupt and most
          importantly clear the interrupt flag so that the interrupt won't
          immediately put us back into the start of the handler again.
      */
      extern "C" void __attribute__((naked, interrupt("IRQ"))) interrupt_vector() {
        DEBUG("interrupt_vector");
        for(;;);
      }

      /**
          @brief The FIQ Interrupt Handler
          The FIQ handler can only be allocated to one interrupt source. The FIQ has
          a full CPU shadow register set. Upon entry to this function the CPU
          switches to the shadow register set so that there is no need to save
          registers before using them in the interrupt.
          In C you can't see the difference between the IRQ and the FIQ interrupt
          handlers except for the FIQ knowing it's source of interrupt as there can
          only be one source, but the prologue and epilogue code is quite different.
          It's much faster on the FIQ interrupt handler.
          The prologue is the code that the compiler inserts at the start of the
          function, if you like, think of the opening curly brace of the function as
          being the prologue code. For the FIQ interrupt handler this is nearly
          empty because the CPU has switched to a fresh set of registers, there's
          nothing we need to save.
          The epilogue is the code that the compiler inserts at the end of the
          function, if you like, think of the closing curly brace of the function as
          being the epilogue code. For the FIQ interrupt handler this is nearly
          empty because the CPU has switched to a fresh set of registers and so has
          not altered the main set of registers.
      */
      extern "C" void __attribute__((naked, interrupt("FIQ"))) fast_interrupt_vector() {
        DEBUG("fast_interrupt_vector");
        for(;;);
      }

      /* Interrupt vectors called by the CPU. Needs to be aligned to 32 bits as the
       * bottom 5 bits of the vector address as set in the control coprocessor must
       * be zero
       *
       * The RESET vector is set to bad_exception. On CPU reset the interrupt vectors
       * are set back to 0x00000000, so it won't be used. Any attempt to call this
       * vector is clearly an error. Also, resetting the Pi will reset VideoCore,
       * and reboot.
       */
      extern "C" __attribute__ ((naked, aligned(32))) void vectors() {
        asm volatile(
          "b reset_vector\n" // ARM4_XRQ_RESET
          "b undefined_instruction_vector\n" // ARM4_XRQ_UNDEF
          "b software_interrupt_vector\n" // ARM4_XRQ_SWINT
          "b prefetch_abort_vector\n" // ARM4_XRQ_ABRTP
          "b data_abort_vector\n" // ARM4_XRQ_ABRTD
          "b bad_exception\n" // ARM4_XRQ_RESV1
          "b interrupt_vector\n" // ARM4_XRQ_IRQ
          "b fast_interrupt_vector\n" // ARM4_XRQ_FIQ
        );
      }

      /* Unhandled exceptions - hang the machine */
      extern "C" __attribute__ ((naked)) void bad_exception() {
        DEBUG("bad exception");
        for(;;);
      }

      /* IRQs flash the OK LED */
      extern "C" __attribute__ ((interrupt ("IRQ"))) void interrupt_irq() {
        DEBUG("interrupt irq");
        for(;;);
      }

      void init() {
        // Set interrupt base register
        DEBUG("zz " << (const void *)&vectors);
        asm volatile("mcr p15, 0, %[addr], c12, c0, 0" : : [addr] "r" (&vectors));

        // Turn on interrupts
        enable();
      }

      void enable() {
        asm volatile("cpsie i");
      }

      void disable() {
        asm volatile("cpsid i");
      }
    }
  }
}
