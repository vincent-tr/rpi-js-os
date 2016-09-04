#include <stddef.h>
#include <stdint.h>

#include "kernel/utils/debug.hh"
#include "kernel/hw/memory/phys-page.hh"
#include "kernel/hw/memory/vm-page.hh"
#include "kernel/platform.hh"
#include "kernel/utils/list.hh"
#include "kernel/utils/panic.hh"
#include "test.hh"

// http://sos.enix.org/wiki-fr/upload/SOSDownload/sos-texte-art3.pdf

#ifdef TEST_ENABLED

namespace test {

  #define MY_PPAGE_NUM_INT 511

  struct ppage : public kernel::utils::list<ppage>::node {
    uint32_t before[MY_PPAGE_NUM_INT];
    uint32_t after[MY_PPAGE_NUM_INT];
  }; /* sizeof() Must be <= 4kB */

  void hw_memory() {
    DEBUG("test begin");

    // We place the pages we did allocate here
    kernel::utils::list<ppage> list;
    ppage *current;
    uint32_t num_alloc_ppages = 0, num_free_ppages = 0;
    const uint32_t &page_size = kernel::platform::get().page_size();
    const uint32_t pages_count = (kernel::platform::get().ram_size() - kernel::platform::get().hw_mem_desc_end()) / page_size;

    DEBUG("page size: " << page_size);
    DEBUG("pages count: " << pages_count);

    while(num_alloc_ppages < pages_count) {
      ++num_alloc_ppages;
      current = reinterpret_cast<ppage*>(static_cast<uint32_t>(kernel::hw::memory::phys_page::alloc()));

      // Print the allocation status
      if(!(num_alloc_ppages % 10000) || num_alloc_ppages == pages_count) {
        DEBUG("Could allocate " << num_alloc_ppages << " pages, last at " << current);
      }

      // We fill this page with its address
      for (int i = 0 ; i < MY_PPAGE_NUM_INT ; i++)
        current->before[i] = current->after[i] = reinterpret_cast<uint32_t>(current);

      // We add this page at the tail of our list of ppages
      list.add(current);
    }

    // Now we release these pages in FIFO order
    while ((current = list.head())) {
      list.remove(current);

      // We make sure this page was not overwritten by any unexpected value
      for (int i = 0 ; i < MY_PPAGE_NUM_INT ; i++) {
        // We don't get what we expect !
        if ((current->before[i] !=  reinterpret_cast<uint32_t>(current))
          || (current->after[i] !=  reinterpret_cast<uint32_t>(current))) {
          // STOP !
          kernel::utils::panic("Page overwritten");
        }
      }


      // Release the descriptor
      kernel::hw::memory::phys_page(reinterpret_cast<uint32_t>(current)).free();

      // Print the deallocation status
      num_free_ppages++;
      if(!(num_free_ppages % 10000) || num_free_ppages == pages_count) {
        DEBUG("Could free " << num_free_ppages << " pages");
      }
    }

    DEBUG("Could allocate " << num_alloc_ppages * page_size
      << " bytes, could free " << num_free_ppages * page_size << " bytes");

    ASSERT(num_alloc_ppages == num_free_ppages);

    DEBUG("test end");
  }

}

#endif // TEST_ENABLED
