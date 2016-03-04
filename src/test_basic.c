#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

#include "judy64nb.h"

void test_construction(void) {
    Judy *j;

    j = judy_open(32, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(j);
    judy_close(j);

    j = judy_open(JUDY_key_size, 8);
    CU_ASSERT_PTR_NOT_NULL_FATAL(j);
    judy_close(j);

    j = judy_open(JUDY_key_size, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(j);
    judy_close(j);
}

void test_fill_binkeys(void) {
    const uint samples = 10000;
    typedef struct {
        uchar data[SHA_DIGEST_LENGTH];
    } _key_t;
    Judy *j;
    JudySlot *slot;
    _key_t *keys;
    uint idx;

    j = judy_open(sizeof(_key_t), 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(j);

    keys = malloc(samples*sizeof(_key_t));
    CU_ASSERT_PTR_NOT_NULL_FATAL(keys);
    for (idx=0; idx<samples; ++idx) {
        unsigned char d[128];
        CU_ASSERT_EQUAL(RAND_bytes(d, sizeof(d)), 1);
        SHA1(d, sizeof(d), keys[idx].data);
    }

    for (idx=0; idx<samples; ++idx) {
        slot = judy_slot(j, keys[idx].data, sizeof(_key_t));
        CU_ASSERT_PTR_NULL_FATAL(slot);
        slot = judy_cell(j, keys[idx].data, sizeof(_key_t));
        CU_ASSERT_PTR_NOT_NULL_FATAL(slot);
        *slot = idx;
        slot = judy_slot(j, keys[idx].data, sizeof(_key_t));
        CU_ASSERT_PTR_NOT_NULL_FATAL(slot);
        CU_ASSERT_EQUAL(*slot, idx);
    }

    for (idx=0; idx<samples; ++idx) {
        slot = judy_slot(j, keys[idx].data, sizeof(_key_t));
        CU_ASSERT_PTR_NOT_NULL_FATAL(slot);
        CU_ASSERT_EQUAL(*slot, idx);
        judy_del(j);
        slot = judy_slot(j, keys[idx].data, sizeof(_key_t));
        CU_ASSERT_PTR_NULL_FATAL(slot);
    }

    for (idx=0; idx<samples; ++idx) {
        slot = judy_slot(j, keys[idx].data, sizeof(_key_t));
        CU_ASSERT_PTR_NULL_FATAL(slot);
    }

    judy_close(j);
}

int init_suite(void) {
    srand((unsigned)time(NULL));

    return 0;
}

int clean_suite(void) {
    return 0;
}

int main(int argc, char **argv) {
   CU_pSuite suite = NULL;
   (void)argc, (void)argv;

   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   if (!(suite = CU_add_suite("basic", init_suite, clean_suite)))
       goto out;

   if (!(CU_add_test(suite, "construction", test_construction)))
       goto out;
   if (!(CU_add_test(suite, "fill_binkeys", test_fill_binkeys)))
       goto out;

   CU_basic_run_tests();

  out:
   CU_cleanup_registry();
   return CU_get_error();
}
