#ifndef KERNEL_UTILS_ARRAY_HH
#define KERNEL_UTILS_ARRAY_HH

// from boost

namespace kernel {
  namespace utils {

    template <typename T, uint32_t N>
    class array {
      T elems[N];

    public:
      // type definitions
      typedef T              value_type;
      typedef T*             iterator;
      typedef const T*       const_iterator;
      typedef T&             reference;
      typedef const T&       const_reference;
      typedef uint32_t       size_type;
      typedef int32_t        difference_type;

      // iterator support
      inline iterator begin() { return elems; }
      inline const_iterator begin() const { return elems; }
      inline iterator end() { return elems+N; }
      inline const_iterator end() const { return elems+N; }

      // operator[]
      inline reference operator[](size_type i) { return elems[i]; }
      inline const_reference operator[](size_type i) const { return elems[i]; }

      // front() and back()
      reference front() { return elems[0]; }
      const_reference front() const { return elems[0]; }
      reference back() { return elems[N-1]; }
      const_reference back() const { return elems[N-1]; }

      // size is constant
      static inline size_type size() { return N; }
      static inline bool empty() { return false; }
      static inline size_type max_size() { return N; }

      // direct access to data (read-only)
      inline const T* data() const { return elems; }

      // use array as C array (direct read/write access to data)
      inline T* data() { return elems; }
    };

    // partial specialization for arrays of size 0
    template <typename T>
    class array<T,0> {
      char c;  // to ensure different array intances return unique values for begin/end

    public:
      // type definitions
      typedef T              value_type;
      typedef T*             iterator;
      typedef const T*       const_iterator;
      typedef T&             reference;
      typedef const T&       const_reference;
      typedef uint32_t       size_type;
      typedef int32_t        difference_type;

      // iterator support
      inline iterator begin() { return reinterpret_cast<iterator>( &c ); }
      inline const_iterator begin() const { return reinterpret_cast<const_iterator>( &c ); }
      inline iterator end() { return reinterpret_cast<iterator>( &c ); }
      inline const_iterator end() const { return reinterpret_cast<const_iterator>( &c ); }

      // size is constant
      static inline size_type size() { return 0; }
      static inline bool empty() { return true; }
      static inline size_type max_size() { return 0; }

      // direct access to data
      inline const T* data() const { return nullptr; }
      inline T* data() { return nullptr; }
    };
  }
}

#endif // KERNEL_UTILS_ARRAY_HH
