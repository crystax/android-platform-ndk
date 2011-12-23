#include "common.h"
#include <crystax/memory.hpp>
#include <crystax/list.hpp>

using ::crystax::list_t;
using ::crystax::scope_cpp_ptr_t;

struct entry_t
{
    int data;
    entry_t *prev;
    entry_t *next;

    entry_t(int d) :data(d), prev(0), next(0) {}
};

int test_list()
{
#ifdef TEST_LIST_CHECK
#undef TEST_LIST_CHECK
#endif
#define TEST_LIST_CHECK(x) \
    if (!(x)) \
    { \
        ::fprintf(stderr, \
            "FAIL at %s:%d: assertion %s failed\n", \
            __FILE__, __LINE__, #x); \
        return 1; \
    } \
    ::printf("ok %d - list\n", __LINE__ - start)

    int start = __LINE__;

    list_t<entry_t> list;
    TEST_LIST_CHECK(list.empty());
    TEST_LIST_CHECK(list.size() == 0);
    TEST_LIST_CHECK(list.head() == 0);
    TEST_LIST_CHECK(list.tail() == 0);

    TEST_LIST_CHECK(list.push(0, 0) == false);

    TEST_LIST_CHECK(list.push_back(new entry_t(1)) == true);
    TEST_LIST_CHECK(list.empty() == false);
    TEST_LIST_CHECK(list.size() == 1);
    TEST_LIST_CHECK(list.head() != 0);
    TEST_LIST_CHECK(list.tail() != 0);
    TEST_LIST_CHECK(list.head() == list.tail());
    TEST_LIST_CHECK(list.head()->data == 1);

    list.clear();
    TEST_LIST_CHECK(list.empty());
    TEST_LIST_CHECK(list.size() == 0);
    TEST_LIST_CHECK(list.head() == 0);
    TEST_LIST_CHECK(list.tail() == 0);

    TEST_LIST_CHECK(list.push_front(new entry_t(1)) == true);
    TEST_LIST_CHECK(list.empty() == false);
    TEST_LIST_CHECK(list.size() == 1);
    TEST_LIST_CHECK(list.head() != 0);
    TEST_LIST_CHECK(list.tail() != 0);
    TEST_LIST_CHECK(list.head() == list.tail());
    TEST_LIST_CHECK(list.head()->data == 1);

    TEST_LIST_CHECK(list.push_back(new entry_t(2)) == true);
    TEST_LIST_CHECK(list.empty() == false);
    TEST_LIST_CHECK(list.size() == 2);
    TEST_LIST_CHECK(list.head() != 0);
    TEST_LIST_CHECK(list.tail() != 0);
    TEST_LIST_CHECK(list.head() != list.tail());
    TEST_LIST_CHECK(list.head()->next == list.tail());
    TEST_LIST_CHECK(list.head() == list.tail()->prev);
    TEST_LIST_CHECK(list.head()->data == 1);
    TEST_LIST_CHECK(list.tail()->data == 2);

    TEST_LIST_CHECK(list.push_back(new entry_t(3)) == true);
    TEST_LIST_CHECK(list.empty() == false);
    TEST_LIST_CHECK(list.size() == 3);
    TEST_LIST_CHECK(list.head() != 0);
    TEST_LIST_CHECK(list.tail() != 0);
    TEST_LIST_CHECK(list.head() == list.tail()->prev->prev);
    TEST_LIST_CHECK(list.head()->next == list.tail()->prev);
    TEST_LIST_CHECK(list.head()->next->next == list.tail());
    TEST_LIST_CHECK(list.head()->data == 1);
    TEST_LIST_CHECK(list.head()->next->data == 2);
    TEST_LIST_CHECK(list.head()->next->next->data = 3);

    TEST_LIST_CHECK(list.push(list.head()->next, new entry_t(4)) == true);
    TEST_LIST_CHECK(list.empty() == false);
    TEST_LIST_CHECK(list.size() == 4);
    TEST_LIST_CHECK(list.head() != 0);
    TEST_LIST_CHECK(list.tail() != 0);
    TEST_LIST_CHECK(list.head() == list.tail()->prev->prev->prev);
    TEST_LIST_CHECK(list.head()->next == list.tail()->prev->prev);
    TEST_LIST_CHECK(list.head()->next->next == list.tail()->prev);
    TEST_LIST_CHECK(list.head()->next->next->next == list.tail());
    TEST_LIST_CHECK(list.head()->data == 1);
    TEST_LIST_CHECK(list.head()->next->data == 2);
    TEST_LIST_CHECK(list.head()->next->next->data == 4);
    TEST_LIST_CHECK(list.head()->next->next->next->data == 3);

    scope_cpp_ptr_t<entry_t> e(list.pop(list.head()->next->next));
    TEST_LIST_CHECK(e);
    TEST_LIST_CHECK(e->data == 4);
    TEST_LIST_CHECK(e->next == 0);
    TEST_LIST_CHECK(e->prev == 0);
    TEST_LIST_CHECK(list.empty() == false);
    TEST_LIST_CHECK(list.size() == 3);
    TEST_LIST_CHECK(list.head() != 0);
    TEST_LIST_CHECK(list.tail() != 0);
    TEST_LIST_CHECK(list.head() == list.tail()->prev->prev);
    TEST_LIST_CHECK(list.head()->next == list.tail()->prev);
    TEST_LIST_CHECK(list.head()->next->next == list.tail());
    TEST_LIST_CHECK(list.head()->data == 1);
    TEST_LIST_CHECK(list.head()->next->data == 2);
    TEST_LIST_CHECK(list.head()->next->next->data = 3);

    e.reset(list.pop_front());
    TEST_LIST_CHECK(e);
    TEST_LIST_CHECK(e->data == 1);
    TEST_LIST_CHECK(e->next == 0);
    TEST_LIST_CHECK(e->prev == 0);
    TEST_LIST_CHECK(list.empty() == false);
    TEST_LIST_CHECK(list.size() == 2);
    TEST_LIST_CHECK(list.head() != 0);
    TEST_LIST_CHECK(list.tail() != 0);
    TEST_LIST_CHECK(list.head() == list.tail()->prev);
    TEST_LIST_CHECK(list.head()->next == list.tail());
    TEST_LIST_CHECK(list.head()->data == 2);
    TEST_LIST_CHECK(list.head()->next->data == 3);

    e.reset(list.pop_back());
    TEST_LIST_CHECK(e);
    TEST_LIST_CHECK(e->data == 3);
    TEST_LIST_CHECK(e->next == 0);
    TEST_LIST_CHECK(e->prev == 0);
    TEST_LIST_CHECK(list.empty() == false);
    TEST_LIST_CHECK(list.size() == 1);
    TEST_LIST_CHECK(list.head() != 0);
    TEST_LIST_CHECK(list.tail() != 0);
    TEST_LIST_CHECK(list.head() == list.tail());
    TEST_LIST_CHECK(list.head()->data == 2);

    e.reset(list.pop(list.head()));
    TEST_LIST_CHECK(e);
    TEST_LIST_CHECK(e->data == 2);
    TEST_LIST_CHECK(e->next == 0);
    TEST_LIST_CHECK(e->prev == 0);
    TEST_LIST_CHECK(list.empty() == true);
    TEST_LIST_CHECK(list.size() == 0);
    TEST_LIST_CHECK(list.head() == 0);
    TEST_LIST_CHECK(list.tail() == 0);

    e.reset(list.pop_back());
    TEST_LIST_CHECK(!e);
    TEST_LIST_CHECK(list.empty() == true);
    TEST_LIST_CHECK(list.size() == 0);
    TEST_LIST_CHECK(list.head() == 0);
    TEST_LIST_CHECK(list.tail() == 0);

#undef TEST_LIST_CHECK

    ::printf("ok\n");

    return 0;
}
