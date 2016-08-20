#ifndef KERNEL_KERNEL_HH
#define KERNEL_KERNEL_HH

#include "kernel/utils/non-copyable.hh"

namespace kernel {
  class platform : public utils::non_copyable {

  public:
    static platform &get();

    void run(const uint32_t &pboot_device, const uint32_t &pmachine_type, const void *patags);

    inline void set_page_size(const uint32_t &value) { page_size = value; }
    inline void set_ram_size(const uint32_t &value) { ram_size = value; }
    inline void set_cmdline(const char *value) { cmdline = value; }

  private:
    uint32_t boot_device;
    uint32_t machine_type;
    uint32_t page_size;
    uint32_t ram_size;
    const char *cmdline;
  };
}

#endif // KERNEL_KERNEL_HH
