#ifndef KERNEL_HW_REGISTERS_HH
#define KERNEL_HW_REGISTERS_HH

namespace kernel {
  namespace hw {
    namespace registers {

      static constexpr uint32_t control_mmu = 1;
      static constexpr uint32_t control_high_exception_vector = 1 << 13;

      static inline uint32_t control_read() {
        uint32_t value;
        asm("mrc p15, 0, %0, c1, c0, 0" : "=r" (value) : : "cc");
        return value;
      }

      static inline void control_write(const uint32_t &value) {
        asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r" (value) : "cc");
      }

      static inline void page_table_write(void *address) {
        asm volatile("mcr p15, 0, %0, c2, c0, 0" : : "r" (address) : "memory");
      }

      static inline void access_control_write(const uint32_t &value) {
        asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (value) : "cc");
      }

      static constexpr uint32_t mode_mask = 0x1f;

      static constexpr uint32_t mode_usr = 0x10;   // Never use this one, as there is no way back!
      static constexpr uint32_t mode_fiq = 0x11;   // banked r8-r14
      static constexpr uint32_t mode_irq = 0x12;
      static constexpr uint32_t mode_svc = 0x13;
      static constexpr uint32_t mode_mon = 0x16;
      static constexpr uint32_t mode_abt = 0x17;
      static constexpr uint32_t mode_und = 0x1B;
      static constexpr uint32_t mode_sys = 0x1F;   // Same as user...

      static inline uint32_t status_read() {
        uint32_t value;
        asm volatile("mrs %0, cpsr" : "=r" (value) : : "cc");
        return value;
      }

      static inline uint32_t mode_get() {
        return status_read() & mode_mask;
      }

      static inline void mode_change(uint32_t mode) {
        uint32_t value = status_read();
        value &= ~mode_mask;
        value |= mode;
        asm volatile("msr cpsr_c, %0" : : "r" (mode) : "cc");
      }
    }
  }
}

#endif // KERNEL_HW_REGISTERS_HH