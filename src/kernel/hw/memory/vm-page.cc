#include <stddef.h>
#include <stdint.h>

#include "phys-page.hh"
#include "kernel/mm/protection.hh"
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

      struct __attribute__ ((packed, aligned (4))) coarse_page_table_descriptor {
        uint32_t type:2;      // set to 1
        uint32_t reserved0:2; // set to 0
        uint32_t reserved1:1; // set to 1
        uint32_t domain:4;
        uint32_t reserved2:1; // set to 0
        uint32_t next_descriptor_base_address:22;
      };

      // [31:12] These bits form the corresponding bits of the physical address.
      // [11:4]  Access permission bits. Domain access control and Fault checking sequence show how to interpret the access permission bits.
      // [3:2]   These bits, C and B, indicate whether the area of memory mapped by this page is treated as write-back cacheable, write-through cacheable, noncached buffered, or noncached nonbuffered.
      // [1:0]   These bits indicate the page size and validity and are interpreted as shown in Table 3.8. -> 1 0 Small page Indicates that this is a 4KB page

      struct __attribute__ ((packed, aligned (4))) small_page_table_descriptor {
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

      struct __attribute__ ((packed, aligned (16384))) first_level_descriptor_table {
        coarse_page_table_descriptor descriptors[4096];
      };

      struct __attribute__ ((packed, aligned (1024))) second_level_descriptor_table {
        small_page_table_descriptor descriptors[256];
      };

      static first_level_descriptor_table first_level_descriptors[1];
      static second_level_descriptor_table second_level_descriptors[4096];

      static void invalidate_tlb();

      static inline small_page_table_descriptor* vaddr_to_descriptor(const uint32_t &vaddr) {
        return second_level_descriptors[vaddr >> 20].descriptors + ((vaddr >> 12) & 0xFF);
      }

      phys_page vm_page::phys() const {
        return phys_page(vaddr_to_descriptor(address)->page_base_address << (32 - 20));
      }

      kernel::mm::protection vm_page::protection() const {
        const auto * desc = vaddr_to_descriptor(address);
        switch(desc->access_permissions_0) {
          case access_permission::read_only:  return kernel::mm::protection{1, 0};
          case access_permission::read_write: return kernel::mm::protection{1, 1};
          default:                            return kernel::mm::protection{0, 0};
        }
      }

      void vm_page::map(const kernel::mm::protection &prot, const phys_page &phys) {
        auto * desc = vaddr_to_descriptor(address);
        access_permission ap = prot.write ? access_permission::read_write : (prot.read ? access_permission::read_only : access_permission::none);
        desc->access_permissions_0 = ap;
        desc->access_permissions_1 = ap;
        desc->access_permissions_2 = ap;
        desc->access_permissions_3 = ap;
        desc->page_base_address = static_cast<uint32_t>(phys) >> (32 - 20);

        bool ram = static_cast<uint32_t>(phys) < kernel::platform::get().ram_size();
        desc->bufferable = desc->cacheable = ram ? 1 : 0;

        invalidate_tlb();
      }

      void vm_page::unmap() {
        auto * desc = vaddr_to_descriptor(address);
        desc->access_permissions_0 = access_permission::none;
        desc->access_permissions_1 = access_permission::none;
        desc->access_permissions_2 = access_permission::none;
        desc->access_permissions_3 = access_permission::none;
        desc->page_base_address = 0;

        invalidate_tlb();
      }

      void vm_page::init() {
        auto *first_desc = first_level_descriptors->descriptors;
        auto *second_table = second_level_descriptors;

        for(;
            first_desc < first_level_descriptors->descriptors + 4096;
            ++first_desc,
            ++second_table) {
          first_desc->type = 1;
          first_desc->reserved0 = 0;
          first_desc->reserved1 = 1;
          first_desc->domain = 0;
          first_desc->reserved2 = 0;
          first_desc->next_descriptor_base_address = reinterpret_cast<uint32_t>(second_table) >> (32 - 22);

          for(auto *second_desc = second_table->descriptors; second_desc < second_table->descriptors + 256; ++second_desc) {
            second_desc->type = 2;
            second_desc->bufferable = 0;
            second_desc->cacheable = 0;
            second_desc->access_permissions_0 = access_permission::none;
            second_desc->access_permissions_1 = access_permission::none;
            second_desc->access_permissions_2 = access_permission::none;
            second_desc->access_permissions_3 = access_permission::none;
            second_desc->page_base_address = 0;
          }
        }
      }

      void vm_page::activate() {
        // Copy the page table address to cp15
        asm volatile("mcr p15, 0, %0, c2, c0, 0" : : "r" (first_level_descriptors) : "memory");
        // Set the access control to all-supervisor
        asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (~0));
        // Enable the MMU
        uint32_t reg;
        asm("mrc p15, 0, %0, c1, c0, 0" : "=r" (reg) : : "cc");
        reg|=0x1;
        asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r" (reg) : "cc");

        invalidate_tlb();
      }

      // TODO: manage read/write accesses propery (set S or R bit then AP 0 = RO and AP != 0 = RW)

      inline void invalidate_tlb() {
        asm volatile("mcr p15, 0, %0, c8, c7, 0" : : "r" (0));
      }
    }
  }
}
