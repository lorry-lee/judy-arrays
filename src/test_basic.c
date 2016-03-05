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

    j = judy_open_bin(0);
    CU_ASSERT_PTR_NULL_FATAL(j);

    j = judy_open_bin(JUDY_key_size+1);
    CU_ASSERT_PTR_NULL_FATAL(j);

    j = judy_open_bin(JUDY_key_size);
    CU_ASSERT_PTR_NOT_NULL_FATAL(j);
    judy_close(j);
}

void test_fill_binkeys(void) {
    const uint samples = 10000;
    typedef struct {
        uchar data[SHA_DIGEST_LENGTH] __attribute__((aligned(JUDY_key_size)));
    } _key_t __attribute__((aligned(JUDY_key_size)));
    unsigned int offset = 0xffff;
    uint *seen;
    Judy *j;
    JudySlot *slot;
    _key_t *keys;
    _key_t last;
    uint idx;

    j = judy_open_bin(sizeof(_key_t));
    CU_ASSERT_PTR_NOT_NULL_FATAL(j);

    seen = malloc(samples*sizeof(uint));
    CU_ASSERT_PTR_NOT_NULL_FATAL(seen);
    memset(seen, 0, samples*sizeof(uint));

    keys = malloc(samples*sizeof(_key_t));
    CU_ASSERT_PTR_NOT_NULL_FATAL(keys);
    memset(keys, 0, samples*sizeof(_key_t));
    for (idx=0; idx<samples; ++idx) {
        unsigned char r[128];
        unsigned char d[SHA_DIGEST_LENGTH];
        CU_ASSERT_EQUAL(RAND_bytes(r, sizeof(r)), 1);
        SHA1(r, sizeof(r), d);
        memcpy(keys[idx].data, d, sizeof(keys[idx].data));
    }

    CU_ASSERT_FALSE_FATAL(judy_key_bin(j, &last));

    idx = 0;
    slot = judy_strt(j, NULL, 0);
    while (slot) {
        ++idx;
        slot = judy_nxt(j);
    }
    CU_ASSERT_EQUAL_FATAL(idx, 0);

    for (idx=0; idx<samples; ++idx) {
        slot = judy_slot_bin(j, keys[idx].data);
        CU_ASSERT_PTR_NULL_FATAL(slot);
        slot = judy_cell_bin(j, keys[idx].data);
        CU_ASSERT_PTR_NOT_NULL_FATAL(slot);
        *slot = idx + offset;
        slot = judy_slot_bin(j, keys[idx].data);
        CU_ASSERT_PTR_NOT_NULL_FATAL(slot);
        CU_ASSERT_EQUAL_FATAL(*slot, idx + offset);
        CU_ASSERT_TRUE_FATAL(judy_key_bin(j, &last));
        CU_ASSERT_EQUAL_FATAL(memcmp(&keys[idx], &last, sizeof(_key_t)), 0);
    }

    for (idx=0; idx<samples; ++idx) {
        slot = judy_slot_bin(j, keys[idx].data);
        CU_ASSERT_PTR_NOT_NULL_FATAL(slot);
        CU_ASSERT_EQUAL_FATAL(*slot, idx + offset);
        CU_ASSERT_TRUE_FATAL(judy_key_bin(j, &last));
        CU_ASSERT_EQUAL_FATAL(memcmp(&keys[idx], &last, sizeof(_key_t)), 0);
    }

    idx = 0;
    slot = judy_strt(j, NULL, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(slot);
    while (slot) {
        ++idx;
        if (slot) {
            ++seen[*slot - offset];
            CU_ASSERT_TRUE_FATAL(judy_key_bin(j, &last));
            CU_ASSERT_EQUAL_FATAL(memcmp(&keys[*slot - offset], &last, sizeof(_key_t)), 0);
        }
        slot = judy_nxt(j);
    }
    CU_ASSERT_EQUAL_FATAL(idx, samples);

    for (idx=0; idx<samples; ++idx) {
        slot = judy_slot_bin(j, keys[idx].data);
        CU_ASSERT_PTR_NOT_NULL_FATAL(slot);
        CU_ASSERT_EQUAL_FATAL(*slot, idx + offset);
        CU_ASSERT_EQUAL_FATAL(seen[idx], 1);
        judy_del(j);
        slot = judy_slot_bin(j, keys[idx].data);
        CU_ASSERT_PTR_NULL_FATAL(slot);
    }

    for (idx=0; idx<samples; ++idx) {
        slot = judy_slot_bin(j, keys[idx].data);
        CU_ASSERT_PTR_NULL_FATAL(slot);
    }

    free(keys);
    free(seen);

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
