#include <hayai/hayai.hpp>

#include <openssl/rand.h>
#include <assert.h>

#include "judy64nb.h"

BENCHMARK(insertion, judy, 10, 10) {
    const uint samples = 10000;
    typedef struct {
        uchar data[20] __attribute__((aligned(JUDY_key_size)));
    } _key_t __attribute__((aligned(JUDY_key_size)));
    typedef struct {
        uchar data[34];
    } _data_t;
    Judy *j;
    JudySlot *slot;
    uint idx;

    j = judy_open_bin(sizeof(_key_t));
    assert(j);

    for (idx=0; idx<samples; ++idx) {
        _key_t k;
        _data_t *d;
        d = (_data_t *)malloc(sizeof(_data_t));
        assert(d);
        RAND_bytes((unsigned char *)&k, sizeof(k));
        slot = judy_cell_bin(j, &k);
        assert(slot);
        *slot = (JudySlot)d;
    }

    slot = judy_strt(j, NULL, 0);
    while (slot) {
        if (slot)
            free((void *)*slot);
        slot = judy_nxt(j);
    }

    judy_close(j);
}

BENCHMARK(search, judy, 10, 10) {
    const uint samples = 10000;
    typedef struct {
        uchar data[20] __attribute__((aligned(JUDY_key_size)));
    } _key_t __attribute__((aligned(JUDY_key_size)));
    typedef struct {
        uint idx;
        uchar data[34];
    } _data_t;
    _key_t *keys;
    Judy *j;
    JudySlot *slot;
    uint idx;

    j = judy_open_bin(sizeof(_key_t));
    assert(j);

    keys = (_key_t *)malloc(samples*sizeof(_key_t));
    assert(keys);
    memset(keys, 0, samples*sizeof(_key_t));

    for (idx=0; idx<samples; ++idx) {
        _data_t *d;
        d = (_data_t *)malloc(sizeof(_data_t));
        assert(d);
        d->idx = idx;
        RAND_bytes((unsigned char *)&keys[idx], sizeof(keys[idx]));
        slot = judy_cell_bin(j, &keys[idx]);
        assert(slot);
        *slot = (JudySlot)d;
    }

    for (idx=0; idx<samples; ++idx) {
        _data_t *d;
        slot = judy_slot_bin(j, &keys[idx]);
        assert(slot);
        d = (_data_t *)(*slot);
        assert(d->idx == idx);
    }

    free(keys);

    slot = judy_strt(j, NULL, 0);
    while (slot) {
        if (slot)
            free((void *)*slot);
        slot = judy_nxt(j);
    }

    judy_close(j);
}

BENCHMARK(remove, judy, 10, 10) {
    const uint samples = 10000;
    typedef struct {
        uchar data[20] __attribute__((aligned(JUDY_key_size)));
    } _key_t __attribute__((aligned(JUDY_key_size)));
    typedef struct {
        uchar data[34];
    } _data_t;
    _key_t *keys;
    Judy *j;
    JudySlot *slot;
    uint idx;

    j = judy_open_bin(sizeof(_key_t));
    assert(j);

    keys = (_key_t *)malloc(samples*sizeof(_key_t));
    assert(keys);
    memset(keys, 0, samples*sizeof(_key_t));

    for (idx=0; idx<samples; ++idx) {
        _data_t *d;
        d = (_data_t *)malloc(sizeof(_data_t));
        assert(d);
        RAND_bytes((unsigned char *)&keys[idx], sizeof(keys[idx]));
        slot = judy_cell_bin(j, &keys[idx]);
        assert(slot);
        *slot = (JudySlot)d;
    }

    for (idx=0; idx<samples; ++idx) {
        slot = judy_slot_bin(j, &keys[idx]);
        assert(slot);
        free((void *)*slot);
        judy_del(j);
    }

    free(keys);

    judy_close(j);
}
