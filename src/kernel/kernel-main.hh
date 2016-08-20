#ifndef KERNEL_KERNEL_HH
#define KERNEL_KERNEL_HH

#include "kernel/utils/non-copyable.hh"

namespace kernel {
  class kernel_main : public utils::non_copyable {

  public:
    static kernel_main &get();

    void run(const uint32_t &pboot_device, const uint32_t &pmachine_type, const void *patags);

  private:
    uint32_t boot_device;
    uint32_t machine_type;
  };
}

#endif // KERNEL_KERNEL_HH
