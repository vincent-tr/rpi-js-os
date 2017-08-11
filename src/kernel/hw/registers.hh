#ifndef KERNEL_HW_REGISTERS_HH
#define KERNEL_HW_REGISTERS_HH

namespace kernel {
  namespace hw {
    namespace registers {

      static constexpr uint32_t control_mmu = 0x1;

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

    }
  }
}

#endif // KERNEL_HW_REGISTERS_HH