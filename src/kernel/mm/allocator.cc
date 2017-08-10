#include <stddef.h>
#include <stdint.h>

#include <string.h>

#include "protection.hh"
#include "region.hh"
#include "allocator.hh"
#include "kernel/utils/placement-new.hh"
#include "kernel/utils/allocator-new.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/list.hh"
#include "kernel/platform.hh"

namespace kernel {
  namespace mm {

    template<uint32_t object_size>
    class object : public utils::list<object<object_size>>::node {
      char padding[ object_size - sizeof(typename utils::list<object<object_size>>::node) ];
    };

    template<>
    class object<8> : public utils::list<object<8>>::node {
    };

    template<uint32_t object_size, uint32_t object_count>
    struct area {
      object<object_size> objects[object_count];
      static_assert(sizeof(objects) == 4096, "wrong area size");
    };

    template<uint32_t object_size, uint32_t object_count, bool is_slab_in_area>
    class slab : public utils::list<slab<object_size, object_count, is_slab_in_area>>::node {
    public:
      typedef slab<object_size, object_count, is_slab_in_area> slab_type;
      typedef area<object_size, object_count> area_type;
      typedef object<object_size> object_type;

    private:
      region *_region;
      area_type *_area;
      utils::list<object_type> free_list;
      uint32_t free_count;

    public:

      explicit inline slab()
       : _region(region::create(object_size * object_count, protection{1, 1}, "kernel:allocator:area"))
       , _area(reinterpret_cast<area_type *>(_region->address()))
       , free_count(0) {

        for(uint32_t i=0; i<object_count; ++i) {
          free_list.add(_area->objects + i);
          ++free_count;
        }
      }

      bool empty() const;

      bool full() const {
        return !free_count;
      }

      bool is_in(void *ptr) const {
        return _region->address() <= reinterpret_cast<uint32_t>(ptr) && reinterpret_cast<uint32_t>(ptr) + object_size <= _region->address_end();
      }

      void *allocate() {
        ASSERT(!full());
        auto *obj = free_list.head();
        free_list.remove(obj);
        --free_count;
        return obj;
      }

      void deallocate(void *ptr) {
        ASSERT(is_in(ptr));
        object_type *obj = reinterpret_cast<object_type *>(ptr);
        free_list.add(obj);
        ++free_count;
      }

      static slab_type *create();
      static void release(slab_type *slab);
    };

    template<uint32_t object_size, uint32_t object_count, bool is_slab_in_area>
    struct slab_manip {
      typedef slab<object_size, object_count, is_slab_in_area> slab_type;

      static inline bool empty(const uint32_t &free_count) {
        return free_count == object_count;
      }

      static inline slab_type *create() {
        return new slab_type();
      }

      static inline void destroy(slab_type *slab) {
        delete slab;
      }
    };

    template<uint32_t object_size, uint32_t object_count>
    struct slab_manip<object_size, object_count, true> {
      typedef slab<object_size, object_count, true> slab_type;

      static inline bool empty(const uint32_t &free_count) {
        return free_count == object_count - 1; // do not count slab
      }

      static inline slab_type *create() {
        static_assert(object_size >= sizeof(slab_type), "bad object_size with is_slab_in_area=true");

        // first init on stack
        char lslab_buffer[ sizeof(slab_type) ];
        slab_type *lslab = reinterpret_cast<slab_type *>(lslab_buffer);
        new (lslab) slab_type();

        // then allocate and move to first object
        slab_type *slab = reinterpret_cast<slab_type *>(lslab->allocate());
        memcpy(slab, lslab, sizeof(slab_type));

        return slab;
      }

      static inline void destroy(slab_type *) {
        // no need to delete, it was allocated on region
      }
    };

    template<uint32_t object_size, uint32_t object_count, bool is_slab_in_area>
    inline bool slab<object_size, object_count, is_slab_in_area>::empty() const {
      return slab_manip<object_size, object_count, is_slab_in_area>::empty(free_count);
    }

    template<uint32_t object_size, uint32_t object_count, bool is_slab_in_area>
    slab<object_size, object_count, is_slab_in_area> *slab<object_size, object_count, is_slab_in_area>::create() {
      return slab_manip<object_size, object_count, is_slab_in_area>::create();
    }

    template<uint32_t object_size, uint32_t object_count, bool is_slab_in_area>
    void slab<object_size, object_count, is_slab_in_area>::release(slab<object_size, object_count, is_slab_in_area> *slab) {
      ASSERT(slab->empty());
      region::release(slab->_region);
      slab_manip<object_size, object_count, is_slab_in_area>::destroy(slab);
    }

    template<uint32_t object_size>
    class cache {
    public:
      static constexpr uint32_t object_count = 4096 / object_size; // should we not hardcode 4096 ?
      static constexpr uint32_t slab_size = sizeof(slab<object_size, object_count, false>);
      static constexpr bool is_slab_in_area = object_size >= slab_size && (object_size / 2 < slab_size);
      static constexpr uint32_t allocation_size = object_size;
      typedef slab<object_size, object_count, is_slab_in_area> slab_type;

    private:
      utils::list<slab_type> slabs;
      slab_type *usable_slab;

      slab_type *find(void *ptr) {
        for(auto *slab : slabs) {
          if(slab->is_in(ptr)) {
            return slab;
          }
        }

        return nullptr;
      }

      void find_usable() {
        if(usable_slab) {
          return;
        }

        for(auto *slab : slabs) {
          if(!slab->full()) {
            usable_slab = slab;
            return;
          }
        }

        usable_slab = slab_type::create();
        slabs.add(usable_slab);
      }

    public:
      explicit inline cache()
       : usable_slab(nullptr) {
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
        slab_type *slab = find(ptr);
        if(!slab) {
          return false;
        }

        slab->deallocate(ptr);

        if(slab->empty()) {
          if(slab == usable_slab) { usable_slab = nullptr; }
          slabs.remove(slab);
          slab_type::release(slab);
        }

        return true;
      }
    };

    template<typename cache_type, typename... next_types>
    class cache_item {
      cache_type cache;
      cache_item<next_types...> next;

    public:
      inline void *allocate(const uint32_t &size) {
        if(size <= cache_type::allocation_size) {
          return cache.allocate();
        }

        return next.allocate(size);
      }

      inline bool deallocate(void *ptr) {
        if(cache.deallocate(ptr)) {
          return true;
        }

        return next.deallocate(ptr);
      }
    };

    template<typename cache_type>
    class cache_item<cache_type> {
      cache_type cache;

    public:
      inline void *allocate(const uint32_t &size) {
        if(size <= cache_type::allocation_size) {
          return cache.allocate();
        }

        return nullptr;
      }

      inline bool deallocate(void *ptr) {
        return cache.deallocate(ptr);
      }
    };

    template<typename... cache_types>
    class cache_set {
      cache_item<cache_types...> caches;
    public:
      inline void *allocate(const uint32_t &size) {
        return caches.allocate(size);
      }

      inline bool deallocate(void *ptr) {
        return caches.deallocate(ptr);
      }
    };

    static cache_set<
      cache<8>,
      cache<16>,
      cache<32>,
      cache<64>,
      cache<128>,
      cache<256>,
      cache<512>,
      cache<1024>,
      cache<2048>
    > caches;

    void allocator::init() {
    }

    void *allocator::allocate(const uint32_t &size) {
      void *ret = caches.allocate(size);
      if(ret) {
        return ret;
      }

      const uint32_t &page_size = kernel::platform::get().page_size();
      uint32_t aligned_size = (size + page_size - 1) / page_size * page_size;
      auto *reg = region::create(aligned_size, protection{1, 1}, "kernel:allocator:big");
      return reinterpret_cast<void*>(reg->address());
    }

    void allocator::deallocate(void *ptr) {
      if(caches.deallocate(ptr)) {
        return;
      }

      auto *reg = region::find(reinterpret_cast<uint32_t>(ptr));
      ASSERT(reg);
      ASSERT(reg->address() == reinterpret_cast<uint32_t>(ptr));
      region::release(reg);
    }
  }
}
