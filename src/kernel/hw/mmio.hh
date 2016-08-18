#ifndef KERNEL_HW_MMIO_HH
#define KERNEL_HW_MMIO_HH

namespace kernel {
  namespace hw {
    namespace mmio {

      static inline void write(const uint32_t &reg, const uint32_t &data) {
        *reinterpret_cast<volatile uint32_t *>(reg) = data;
      }

      static inline uint32_t read(const uint32_t &reg) {
        return *reinterpret_cast<volatile const uint32_t *>(reg);
      }

    }
  }
}

#endif // KERNEL_HW_MMIO_HH