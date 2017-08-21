#include <stddef.h>
#include <stdint.h>

#include <string.h>

#include "exceptions.hh"
#include "kernel/utils/debug.hh"
#include "kernel/mm/protection.hh"
#include "kernel/mm/region.hh"
#include "kernel/platform.hh"
#include "kernel/hw/registers.hh"

// https://github.com/brianwiddas/pi-baremetal/blob/master/interrupts.c
// https://github.com/BrianSidebotham/arm-tutorial-rpi/blob/master/part-4/armc-013/rpi-interrupts.c
// https://stackoverflow.com/questions/21312963/arm-bootloader-interrupt-vector-table-understanding
// https://stackoverflow.com/questions/22295566/how-can-i-put-arm-processor-in-different-modes-using-c-program

namespace kernel {
  namespace hw {
    namespace exceptions {

      /**
          @brief The Reset vector interrupt handler
          This can never be called, since an ARM core reset would also reset the
          GPU and therefore cause the GPU to start running code again until
          the ARM is handed control at the end of boot loading
      */
      extern "C" void __attribute__((interrupt("ABORT"))) reset_vector() {
        DEBUG("reset_vector");
        for(;;);
      }

      /**
          @brief The undefined instruction interrupt handler
          If an undefined intstruction is encountered, the CPU will start
          executing this function. Just trap here as a debug solution.
      */
      extern "C" void __attribute__((interrupt("UNDEF"))) undefined_instruction_vector() {
        DEBUG("undefined_instruction_vector");
        for(;;);
      }

      /**
          @brief The supervisor call interrupt handler
          The CPU will start executing this function. Just trap here as a debug
          solution.
      */
      extern "C" void __attribute__((interrupt("SWI"))) software_interrupt_vector() {
        DEBUG("software_interrupt_vector");
        for(;;);
      }

      /**
          @brief The prefetch abort interrupt handler
          The CPU will start executing this function. Just trap here as a debug
          solution.
      */
      extern "C" void __attribute__((interrupt("ABORT"))) prefetch_abort_vector() {

        register uint32_t addr;
        asm volatile("mov %[addr], lr" : [addr] "=r" (addr) );

        DEBUG("prefetch_abort_vector at " << reinterpret_cast<void*>(addr));
        for(;;);
      }

      /**
          @brief The Data Abort interrupt handler
          The CPU will start executing this function. Just trap here as a debug
          solution.
      */
      extern "C" void __attribute__((interrupt("ABORT"))) data_abort_vector() {

        register uint32_t addr, far;
        asm volatile("mov %[addr], lr" : [addr] "=r" (addr) );
        /* Read fault address register */
        asm volatile("mrc p15, 0, %[addr], c6, c0, 0": [addr] "=r" (far) );

        DEBUG("data_abort_vector at " << reinterpret_cast<void*>(addr - 4) << " accessing address " << reinterpret_cast<void*>(far));
        for(;;);
      }

      /**
          @brief The IRQ Interrupt handler
          This handler is run every time an interrupt source is triggered. It's
          up to the handler to determine the source of the interrupt and most
          importantly clear the interrupt flag so that the interrupt won't
          immediately put us back into the start of the handler again.
      */
      extern "C" void __attribute__((interrupt("IRQ"))) interrupt_vector() {
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
      extern "C" void __attribute__((interrupt("FIQ"))) fast_interrupt_vector() {
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
      static __attribute__ ((naked, section(".text.exceptions"))) void vectors() {
        asm volatile(
          "ldr pc, preset_vector\n"                 // ARM4_XRQ_RESET
          "ldr pc, pundefined_instruction_vector\n" // ARM4_XRQ_UNDEF
          "ldr pc, psoftware_interrupt_vector\n"    // ARM4_XRQ_SWINT
          "ldr pc, pprefetch_abort_vector\n"        // ARM4_XRQ_ABRTP
          "ldr pc, pdata_abort_vector\n"            // ARM4_XRQ_ABRTD
          "ldr pc, preset_vector\n"                 // ARM4_XRQ_RESV1
          "ldr pc, pinterrupt_vector\n"             // ARM4_XRQ_IRQ
          "ldr pc, pfast_interrupt_vector\n"        // ARM4_XRQ_FIQ
          "preset_vector: .word reset_vector\n"
          "pundefined_instruction_vector: .word undefined_instruction_vector\n"
          "psoftware_interrupt_vector: .word software_interrupt_vector\n"
          "pprefetch_abort_vector: .word prefetch_abort_vector\n"
          "pdata_abort_vector: .word data_abort_vector\n"
          "pinterrupt_vector: .word interrupt_vector\n"
          "pfast_interrupt_vector: .word fast_interrupt_vector\n"
        );
      }

      extern "C" uint32_t __text_exceptions_start;
      extern "C" uint32_t __text_exceptions_end;

      /*static*/ void setup_stacks(const void *stack) {
        registers::mode_change(registers::mode_fiq);
        asm volatile("mov sp, %0" : : "r" (stack) : "memory");
        registers::mode_change(registers::mode_irq);
        asm volatile("mov sp, %0" : : "r" (stack) : "memory");
        registers::mode_change(registers::mode_abt);
        asm volatile("mov sp, %0" : : "r" (stack) : "memory");
        registers::mode_change(registers::mode_sys);
        asm volatile("mov sp, %0" : : "r" (stack) : "memory");
        registers::mode_change(registers::mode_und);
        asm volatile("mov sp, %0" : : "r" (stack) : "memory");
        registers::mode_change(registers::mode_svc);
      }

      void init() {

        constexpr uint32_t vectors_address = 0xFFFF0000;
        const uint32_t &page_size = kernel::platform::get().page_size();
        const uint32_t vectors_size = reinterpret_cast<uint32_t>(&__text_exceptions_end) - reinterpret_cast<uint32_t>(&__text_exceptions_start);
        kernel::mm::region::create(vectors_address, page_size, kernel::mm::protection{1,1}, "kernel:exceptions");
        DEBUG("memcpy " << reinterpret_cast<void*>(vectors_address) << " " << reinterpret_cast<void*>(vectors) << " " << vectors_size);
        memcpy(reinterpret_cast<void*>(vectors_address), &__text_exceptions_start, vectors_size);

        uint32_t control = registers::control_read();
        control |= registers::control_high_exception_vector;
        registers::control_write(control);

        const kernel::mm::region *exc_stack = kernel::mm::region::get_exc_stack();
        void *stack_ptr = reinterpret_cast<void*>(exc_stack->address_end());
        setup_stacks(stack_ptr);

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
