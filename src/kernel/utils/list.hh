#ifndef KERNEL_UTILS_LIST_HH
#define KERNEL_UTILS_LIST_HH

namespace kernel {
  namespace utils {

    template<typename T>
    class list {
      T *list_head;

    public:
      class node {
        T *prev;
        T *next;

        friend list<T>;
      };

      class iterator_type {
        T *current;
        T *last;

      public:
        explicit inline iterator_type(T *head)
         : current(head),
           last(head ? head->prev : nullptr) {
        }

        inline iterator_type& operator++() {
          if(current == last) {
            current = nullptr;
            return *this;
          }
          current = current->next;
          return *this;
        }

        inline operator bool() const {
          return current;
        }

        inline T *operator*() const {
          return current;
        }

        inline T *operator->() const {
          return current;
        }

        inline bool operator==(const iterator_type &other) const {
          return current = other.current;
        }
      };

      class const_iterator_type {
        const T *current;
        const T *last;

      public:
        explicit inline const_iterator_type(const T *head)
         : current(head),
           last(head ? head->prev : nullptr) {
        }

        inline const_iterator_type& operator++() {
          if(current == last) {
            current = nullptr;
            return *this;
          }
          current = current->next;
          return *this;
        }

        inline operator bool() const {
          return current;
        }

        inline const T *operator*() const {
          return current;
        }

        inline const T *operator->() const {
          return current;
        }

        inline bool operator==(const const_iterator_type &other) const {
          return current = other.current;
        }
      };

      explicit inline list()
       : list_head(nullptr) {
      }

      inline void add(T *node) {
        if(!list_head) {
          list_head = node->prev = node->next = node;
          return;
        }

        auto last = list_head->prev;
        last->next = node;
        node->prev = last;
        list_head->prev = node;
        node->next = list_head;
      }

      inline void remove(T *node) {
        node->next->prev = node->prev;
        node->prev->next = node->next;
        if(list_head == node) {
          list_head = node->next;
        }
        if(list_head == node) {
          list_head = nullptr;
        }
      }

      inline T *head() {
        return list_head;
      }

      inline const T *head() const {
        return list_head;
      }

      inline T *tail() {
        if(!list_head) { return nullptr; }
        return list_head->prev;
      }

      inline const T *tail() const {
        if(!list_head) { return nullptr; }
        return list_head->prev;
      }

      inline bool empty() const {
        return !list_head;
      }

      inline uint32_t count() const {
        if(!list_head) { return 0; }
        uint32_t count = 1;
        for(T *current = list_head->next; current != list_head; current = current->next, ++count);
        return count;
      }

      inline iterator_type begin() {
        return iterator_type(list_head);
      }

      inline const_iterator_type begin() const {
        return const_iterator_type(list_head);
      }

      inline iterator_type end() {
        return iterator_type(nullptr);
      }

      inline const_iterator_type end() const {
        return const_iterator_type(nullptr);
      }
    };


  }
}

#endif // KERNEL_UTILS_LIST_HH
