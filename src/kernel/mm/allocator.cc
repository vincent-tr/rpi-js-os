#include <stddef.h>
#include <stdint.h>

#include "protection.hh"
#include "region.hh"
#include "allocator.hh"
#include "kernel/utils/placement-new.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/list.hh"
#include "kernel/platform.hh"

namespace kernel {
  namespace mm {

    static constexpr uint32_t slab_min_objects = 16;

    struct free_item : public utils::list<free_item>::node {
    };

    class slab : public utils::list<slab>::node {
      region *_region;
      uint32_t obj_size;
      uint32_t free_count;
      utils::list<free_item> free_objects;

    public:
      explicit inline slab(const uint32_t &pobj_size)
       : obj_size(pobj_size)
       , free_count(0) {
        const uint32_t &page_size = kernel::platform::get().page_size();
        uint32_t size = slab_min_objects * obj_size;
        if(size < page_size) {
          size = page_size;
        }

        _region = region::create(size, protection{1, 1}, "kernel:allocator:slab");
        for(uint32_t addr = _region->address(); addr < _region->address_end(); addr += obj_size) {
          free_item *obj = reinterpret_cast<free_item *>(addr);
          free_objects.add(obj);
          ++free_count;
        }
      }

      bool empty() const {
        return free_count * obj_size == _region->length();
      }

      bool full() const {
        return !free_count;
      }

      bool is_in(void *ptr) {
        return _region->address() <= reinterpret_cast<uint32_t>(ptr) && reinterpret_cast<uint32_t>(ptr) + obj_size < _region->address_end();
      }

      void *allocate() {
        ASSERT(!full());
        auto *obj = free_objects.head();
        free_objects.remove(obj);
        --free_count;
        return obj;
      }

      void deallocate(void *ptr) {
        ASSERT(is_in(ptr));
        free_item *obj = reinterpret_cast<free_item *>(ptr);
        free_objects.add(obj);
        ++free_count;
      }
    };

    class cache {
      uint32_t obj_size;
      utils::list<slab> slabs;
      slab *usable_slab;

      slab *find(void *ptr) {
        for(auto *s : slabs) {
          if(s->is_in(ptr)) {
            return s;
          }
        }

        return nullptr;
      }

      void find_usable() {
        if(usable_slab) {
          return;
        }

        for(auto *s : slabs) {
          if(!s->full()) {
            usable_slab = s;
            return;
          }
        }

        // TODO: alloc slab
      }

    public:
      explicit inline cache(const uint32_t &pobject_size)
       : obj_size(pobject_size)
       , usable_slab(nullptr) {
      }

      inline const uint32_t &object_size() const {
        return obj_size;
      }

      void *allocate() {
        find_usable();

        void *ret = usable_slab->allocate();

        if(usable_slab->full()) {
          usable_slab = nullptr;
        }

        return ret;
      }

      bool deallocate(void *ptr) {
        slab *s = find(ptr);
        if(!s) {
          return false;
        }

        s->deallocate(ptr);

        if(s->empty()) {
          // TODO: free slab
        }

        return true;
      }
    };

    static cache caches[] = {
      cache(8),
      cache(16),
      cache(32),
      cache(64),
      cache(128),
      cache(256),
      cache(512),
      cache(1024),
      cache(2048)
    };

    static constexpr uint32_t caches_size = sizeof(caches) / sizeof(cache);

    void allocator::init() {
    }

    void *allocator::allocate(const uint32_t &size) {
      for(uint32_t i = 0; i < caches_size; ++i) {
        auto &cache = caches[i];
        if(size <= cache.object_size()) {
          return cache.allocate();
        }
      }

      const uint32_t &page_size = kernel::platform::get().page_size();
      uint32_t aligned_size = (size + page_size - 1) / page_size * page_size;
      auto *reg = region::create(aligned_size, protection{1, 1}, "kernel:allocator:big");
      return reinterpret_cast<void*>(reg->address());
    }

    void allocator::deallocate(void *ptr) {
      for(uint32_t i = 0; i < caches_size; ++i) {
        auto &cache = caches[i];
        if(cache.deallocate(ptr)) {
          return;
        }
      }

      auto *reg = region::find(reinterpret_cast<uint32_t>(ptr));
      ASSERT(reg);
      ASSERT(reg->address() == reinterpret_cast<uint32_t>(ptr));
      region::release(reg);
    }
  }
}
