#ifndef KERNEL_UTILS_NON_COPYABLE_HH
#define KERNEL_UTILS_NON_COPYABLE_HH

namespace kernel {
  namespace utils {

    class non_copyable {
    protected:
      constexpr non_copyable() = default;
      ~non_copyable()          = default;

      non_copyable(const non_copyable&) = delete;
      non_copyable& operator=(const non_copyable&) = delete;
    };
  }
}

#endif // KERNEL_UTILS_NON_COPYABLE_HH
