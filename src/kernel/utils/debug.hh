#ifndef KERNEL_UTILS_DEBUG_HH
#define KERNEL_UTILS_DEBUG_HH

#include "kernel/utils/non-copyable.hh"

#ifdef QEMU
#define DEBUG_ENABLED
#endif

#ifdef DEBUG_ENABLED
#define DEBUG(expr) (kernel::utils::details::debug(__FILE__, __LINE__) << expr)

#define ASSERT(expr) ((void)((expr) || ( \
  (kernel::utils::details::debug(__FILE__, __LINE__) << "assertion failed: " << #expr).fatal(), \
  false)))

namespace kernel {
  namespace utils {
    namespace details {

      class debug : utils::non_copyable {
      public:
        explicit debug(const char *file, const int &line);
        ~debug();

        debug &operator << (const char *str);
        debug &operator << (const int &val);
        debug &operator << (const int8_t &val);
        debug &operator << (const int16_t &val);
        debug &operator << (const int32_t &val);
        debug &operator << (const unsigned int &val);
        debug &operator << (const uint8_t &val);
        debug &operator << (const uint16_t &val);
        debug &operator << (const uint32_t &val);
        debug &operator << (const void *ptr);

        void fatal();
      };
    }
  }
}

#else // DEBUG_ENABLED
#define DEBUG(expr)
#define ASSERT(expr)
#endif // DEBUG_ENABLED

#endif // KERNEL_UTILS_DEBUG_HH
