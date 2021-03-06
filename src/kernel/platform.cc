#include <stddef.h>
#include <stdint.h>

#include "platform.hh"

#include "kernel/hw/uart.hh"
#include "kernel/hw/atags.hh"
#include "kernel/hw/exceptions.hh"
#include "kernel/hw/memory/phys-page.hh"
#include "kernel/mm/protection.hh"
#include "kernel/hw/memory/vm-page.hh"
#include "kernel/mm/region.hh"
#include "kernel/mm/allocator.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/panic.hh"
#include "test/test.hh"

namespace kernel {

  static void start_thread(void *stack, void(*entry)());

  void platform::parse_atags() {
    for(hw::atags::reader r; r; ++r) {
      switch(r->tag) {
        case hw::atags::type::CORE: {
          auto tag = static_cast<const hw::atags::core *>(*r);
          _page_size = tag->pagesize;
          break;
        }

        case hw::atags::type::MEM: {
          auto tag = static_cast<const hw::atags::mem *>(*r);
          _ram_size = tag->size;
          ASSERT(tag->address == 0);
          break;
        }

        case hw::atags::type::SERIAL: {
          auto tag = static_cast<const hw::atags::serial *>(*r);
          _serial_low = tag->low;
          _serial_high = tag->high;
          break;
        }

        case hw::atags::type::REVISION: {
          auto tag = static_cast<const hw::atags::revision *>(*r);
          _revision = tag->revision;
          break;
        }

        case hw::atags::type::CMDLINE: {
          auto tag = static_cast<const hw::atags::cmdline *>(*r);
          _cmdline = tag->commandline;
          break;
        }

        default:
          break;
      }
    }
  }

  void platform::test() {
    test::utils_list();
    test::hw_memory();
    test::mm();
  }

  static platform instance;

  platform &platform::get() {
    return instance;
  }

  void platform::run(const uint32_t &pboot_device, const uint32_t &pmachine_type, const void *patags) {
    _boot_device = pboot_device;
    _machine_type = pmachine_type;

    kernel::hw::uart::init();
    kernel::hw::atags::init(patags);
    parse_atags();

    kernel::hw::memory::phys_page::init(_hw_mem_desc_begin, _hw_mem_desc_end);
    kernel::hw::memory::vm_page::init();
    kernel::mm::region::init_bootstrap();
    kernel::hw::memory::vm_page::activate();
    kernel::mm::allocator::init();

    kernel::hw::exceptions::init();

    start_thread(reinterpret_cast<void*>(kernel::mm::region::get_stack()->address_end()), thread_entry);
  }

  void start_thread(void *stack, void(*entry)()) {
    asm volatile("mov sp, %0" : : "r" (stack) : "memory");
    asm volatile("mov r3, %0" : : "r" (entry) : "memory");
    asm volatile("blx r3");

    for(;;);
  }

  void platform::thread_entry() {
    platform::get().thread_main();
  }

  void platform::thread_main() {

    kernel::mm::region::init_finalize();

    try {

      test();

      DEBUG("Hello, kernel World!");

    } catch(...) {
      kernel::utils::panic("uncaught error");
    }
  }
}
