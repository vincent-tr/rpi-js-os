#ifndef KERNEL_UTILS_DEBUG_HH
#define KERNEL_UTILS_DEBUG_HH

#include "kernel/utils/non_copyable.hh"

#ifdef QEMU
#define DEBUG_ENABLED
#endif

#ifdef DEBUG_ENABLED
#define DEBUG(expr) (kernel::utils::details::debug(__FILE__, __LINE__) << expr)

namespace kernel {
  namespace utils {
    namespace details {

      class debug : utils::non_copyable {
      public:
        explicit debug(const char *file, const int &line);
        ~debug();

        debug &operator << (const char *str);
      };
    }
  }
}

#else // DEBUG_ENABLED
#define DEBUG(expr)
#endif // DEBUG_ENABLED

#endif // KERNEL_UTILS_DEBUG_HH
