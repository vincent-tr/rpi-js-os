#ifndef KERNEL_KERNEL_HH
#define KERNEL_KERNEL_HH

#include "kernel/utils/non-copyable.hh"

namespace kernel {
  class platform : public utils::non_copyable {

    void parse_atags();
    void thread_main();
    void test();

    static void thread_entry();

  public:
    static platform &get();

    void run(const uint32_t &pboot_device, const uint32_t &pmachine_type, const void *patags);

  private:
    uint32_t _boot_device;
    uint32_t _machine_type;
    uint32_t _page_size;
    uint32_t _ram_size;
    const char *_cmdline;
    uint32_t _serial_low;
    uint32_t _serial_high;
    uint32_t _revision;
    uint32_t _hw_mem_desc_begin;
    uint32_t _hw_mem_desc_end;

  public:
    inline const uint32_t &boot_device() const { return _boot_device; }
    inline const uint32_t &machine_type() const { return _machine_type; }
    inline const uint32_t &page_size() const { return _page_size; }
    inline const uint32_t &ram_size() const { return _ram_size; }
    inline const char *cmdline() const { return _cmdline; }
    inline const uint32_t &serial_low() const { return _serial_low; }
    inline const uint32_t &serial_high() const { return _serial_high; }
    inline const uint32_t &revision() const { return _revision; }
    inline const uint32_t &hw_mem_desc_begin() const { return _hw_mem_desc_begin; }
    inline const uint32_t &hw_mem_desc_end() const { return _hw_mem_desc_end; }
  };
}

#endif // KERNEL_KERNEL_HH
