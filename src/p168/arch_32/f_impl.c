/* Copyright (c) 2014 Cryptography Research, Inc.
 * Released under the MIT License.  See LICENSE.txt for license information.
 */

#ifdef __i386__

#include "f_field.h"

void gf_mul(gf_s* DECAF_RESTRICT cs, const gf as, const gf bs) {
    const uint32_t *a = as->limb, *b = bs->limb, mask = ((1 << 21) - 1);

    uint32_t bh[7];
    int i, j;
    for (i = 0; i<7; i++) bh[i] = widemul(b[i + 1], 257);

    uint32_t *c = cs->limb;

    uint64_t accum = 0;
    for (i = 0; i<8; i++) {
        for (j = 0; j <= i; j++) {
            accum += widemul(b[i - j], a[j]);
        }
        for (; j<8; j++) {
            accum += widemul(bh[i - j + 7], a[j]);
        }
        c[i] = accum & mask;
        accum >>= 21;
    }

    accum *= 257;
    accum += c[0];
    c[0] = accum & mask;
    accum >>= 21;

    assert(accum < mask);
    c[1] += (uint32_t)accum;
}

void gf_mulw_unsigned(gf_s* DECAF_RESTRICT cs, const gf as, uint32_t b) {
    const uint32_t *a = as->limb, mask = ((1 << 21) - 1);
    uint32_t *c = cs->limb;
    uint64_t accum = widemul(b, a[0]);
    c[0] = accum & mask;
    accum >>= 21;
    accum += widemul(b, a[1]);
    c[1] = accum & mask;
    accum >>= 21;

    for (int i = 2; i<8;) {
        accum += widemul(b, a[i]);
        c[i] = accum & mask;
        accum >>= 21;
        i++;
    }

    accum *= 257;
    accum += c[0];
    c[0] = accum & mask;
    accum >>= 21;

    assert(accum < mask);
    c[1] += (uint32_t)accum;
}

void gf_sqr (gf_s* DECAF_RESTRICT cs, const gf as) {
    gf_mul(cs,as,as); /* Performs better with a dedicated square */
}

#endif // __i386__
