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
        bool valid;

      public:
        explicit iterator_type(T *head)
         : current(head),
           last(head ? head->prev : nullptr),
           valid(head) {
        }

        iterator_type& operator++() {
          if(current == last) {
            valid = false;
          }
          current = current->next;
          return *this;
        }

        inline operator bool() const {
          return valid;
        }

        T *operator*() const {
          return current;
        }

        T *operator->() const {
          return current;
        }
      };

      class const_iterator_type {
        const T *current;
        const T *last;
        bool valid;

      public:
        explicit const_iterator_type(const T *head)
         : current(head),
           last(head ? head->prev : nullptr),
           valid(head) {
        }

        const_iterator_type& operator++() {
          if(current == last) {
            valid = false;
          }
          current = current->next;
          return *this;
        }

        inline operator bool() const {
          return valid;
        }

        const T *operator*() const {
          return current;
        }

        const T *operator->() const {
          return current;
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

      inline iterator_type iterator() {
        return iterator_type(list_head);
      }

      inline const_iterator_type iterator() const {
        return const_iterator_type(list_head);
      }
    };


  }
}

#endif // KERNEL_UTILS_LIST_HH
