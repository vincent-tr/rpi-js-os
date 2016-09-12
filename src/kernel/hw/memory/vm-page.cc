#include <stddef.h>
#include <stdint.h>

#include "phys-page.hh"
#include "vm-page.hh"
#include "kernel/utils/placement-new.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/list.hh"
#include "kernel/platform.hh"

namespace kernel {
  namespace hw {
    namespace memory {

      // http://infocenter.arm.com/help/topic/com.arm.doc.ddi0198e/Babejahc.html

      // [31:10] These bits form the base for referencing the second-level descriptor (the coarse page table index for the entry is derived from the MVA)
      // [9]     Always written as 0
      // [8:5]   These bits specify one of the 16 possible domains, held in the domain access control registers, that contain the primary access controls
      // [4]     Always written as 1
      // [3:2]   Always written as 0
      // [1:0]   These bits must be 01 to indicate a coarse page table descriptor

      __attribute__ ((packed, aligned (4))) struct coarse_page_table_descriptor {
        uint32_t type:2;      // set to 1
        uint32_t reserved0:3; // set to 4
        uint32_t domain:4;
        uint32_t reserved1:1; // set to 0
        uint32_t next_descriptor_base_address:22;
      };

      // [31:12] These bits form the corresponding bits of the physical address.
      // [11:4]  Access permission bits. Domain access control and Fault checking sequence show how to interpret the access permission bits.
      // [3:2]   These bits, C and B, indicate whether the area of memory mapped by this page is treated as write-back cacheable, write-through cacheable, noncached buffered, or noncached nonbuffered.
      // [1:0]   These bits indicate the page size and validity and are interpreted as shown in Table 3.8. -> 1 0 Small page Indicates that this is a 4KB page

      __attribute__ ((packed, aligned (4))) struct small_page_table_descriptor {
        uint32_t type:2; // set to 2
        uint32_t bufferable:1;
        uint32_t cacheable:1;
        uint32_t access_permissions_0:2;
        uint32_t access_permissions_1:2;
        uint32_t access_permissions_2:2;
        uint32_t access_permissions_3:2;
        uint32_t page_base_address:20;
      };

      // Permissions:

      // http://infocenter.arm.com/help/topic/com.arm.doc.ddi0198e/Babegcjf.html

      // AP S R Privileged permissions  User permissions
      // 00 0 0 No access               No access
      // 00 1 0 Read-only               No access
      // 00 0 1 Read-only               Read-only
      // 00 1 1 Unpredictable           Unpredictable
      // 01 x x Read/write              No access
      // 10 x x Read/write              Read-only
      // 11 x x Read/write              Read/write

      enum access_permission {
        none = 1,
        read_only = 2,
        read_write = 3
      };

      // Domains:
      // http://infocenter.arm.com/help/topic/com.arm.doc.ddi0198e/Babegcjf.html

      // Cache (bufferable, cacheable)
      // http://infocenter.arm.com/help/topic/com.arm.doc.ddi0198e/Cacddigd.html

      phys_page vm_page::phys() const {
        // TODO
      }

      const vm_protection &vm_page::protection() const {
        // TODO
      }

      void vm_page::map(const vm_protection &prot, const phys_page &phys) {
        // TODO
      }

      void vm_page::unmap() {
        // TODO
      }

      void vm_page::init(uint32_t &desc_end) {
        // TODO
      }
    }
  }
}
