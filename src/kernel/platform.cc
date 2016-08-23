#include <stddef.h>
#include <stdint.h>

#include "platform.hh"

#include "kernel/hw/uart.hh"
#include "kernel/hw/atags.hh"
#include "kernel/hw/interrupts.hh"
#include "kernel/hw/memory.hh"
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
    test::hw_memory();
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

    kernel::hw::interrupts::init();
    kernel::hw::memory::init(_hw_mem_desc_begin, _hw_mem_desc_end);

    test();

    DEBUG("Hello, kernel World!");

    DEBUG("TODO: tests on phys mem");
  }
}