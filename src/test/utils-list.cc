#include <stddef.h>
#include <stdint.h>

#include "kernel/utils/debug.hh"
#include "kernel/utils/list.hh"
#include "kernel/utils/panic.hh"
#include "test.hh"

#ifdef TEST_ENABLED

namespace test {

  class node_test : public kernel::utils::list<node_test>::node {
    int _value;

  public:
    explicit inline node_test(const int &val)
     : _value(val) {
    }

    const int &value() const { return _value; }
  };

  void utils_list() {
    DEBUG("test begin");

    kernel::utils::list<node_test> list;

    node_test val1(1);
    node_test val2(2);
    node_test val3(3);

    ASSERT(list.empty());
    ASSERT(list.count() == 0);

    list.add(&val1);

    ASSERT(!list.empty());
    ASSERT(list.count() == 1);

    list.remove(&val1);

    ASSERT(list.empty());
    ASSERT(list.count() == 0);

    list.add(&val1);
    list.add(&val2);
    list.add(&val3);

    ASSERT(!list.empty());
    ASSERT(list.count() == 3);

    int i=0;
    for(auto node : list) {
      ASSERT(node->value() == ++i);
    }

    list.remove(&val2);
    list.remove(&val3);
    list.remove(&val1);

    ASSERT(list.empty());
    ASSERT(list.count() == 0);

    list.add(&val2);
    list.insert_before(&val2, &val1);
    list.insert_after(&val2, &val3);

    i=0;
    for(auto node : list) {
      ASSERT(node->value() == ++i);
    }

    list.remove(&val1);
    list.remove(&val2);
    list.remove(&val3);

    ASSERT(list.empty());

    list.add(&val2);
    list.insert_head(&val1);
    list.insert_tail(&val3);

    i=0;
    for(auto node : list) {
      ASSERT(node->value() == ++i);
    }

    list.remove(&val1);
    list.remove(&val2);
    list.remove(&val3);

    ASSERT(list.empty());

    DEBUG("test end");
  }

}

#endif // TEST_ENABLED
