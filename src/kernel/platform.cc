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
#include "test/test.hh"

namespace kernel {

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
    kernel::mm::region::init();
    kernel::hw::memory::vm_page::activate();
    kernel::mm::allocator::init();

    // TODO: move stack

    kernel::hw::exceptions::init();

    DEBUG("kernel memory layout");
    for(auto *region = kernel::mm::region::get_first(); region; region = kernel::mm::region::get_next(region)) {
      DEBUG(
        "region: " <<
        reinterpret_cast<void*>(region->address()) << " -> " << reinterpret_cast<void*>(region->address_end()) <<
        " (" << region->length() << ") : " << region->name());
    }

    test();

    DEBUG("Hello, kernel World!");
  }
}
