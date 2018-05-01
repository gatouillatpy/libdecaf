/**
 * @cond internal
 * @file f_arithmetic.c
 * @copyright
 *   Copyright (c) 2014 Cryptography Research, Inc.  \n
 *   Released under the MIT License.  See LICENSE.txt for license information.
 * @author Mike Hamburg
 * @brief Field-specific arithmetic.
 */

#include "field.h"

mask_t gf_isr (
    gf a,
    const gf x
) {
    gf x2, x3;
    gf L0, L1, L2;
    gf M0, M1;
    gf T0, T1, T2;
    
    gf_sqr(x2, x); // 2
    gf_mul(L0, x, x2); // 2 + 1
    gf_sqr(L1, x2); // 2**2
    gf_mul(T1, L0, L1); // 2**2 + 2 + 1
    gf_sqr(x3, L1); // 2**3
    gf_mul(L1, x3, T1); // 2**3 + T1
    gf_sqrn(T0, L1, 2); // 2**2 * (2**3 + T1)
    gf_mul(L2, L0, T0); // L0 + T0
    gf_sqr(T2, T0); // 2**3 * (2**3 + T1)
    gf_mul(M0, T1, T2); // T1 + T2
    gf_sqrn(L1, x3, 4); // 2**7
    gf_mul(L0, L1, T2); // 2**7 + T2
    gf_sqrn(T1, L0, 4); // 2**4 * (2**7 + T2)
    gf_mul(M1, M0, T1); // M0 + T1
    gf_sqrn(T0, M1, 7); // 2**7 * M1
    gf_mul(L0, L2, T0); // L2 + T0
    gf_mul(L2, T2, T1); // T2 + T1
    gf_sqrn(T2, L2, 9); // 2**9 * (T2 + T1)
    gf_mul(M0, M1, T2); // M1 + T2
    gf_sqrn(T0, M0, 19); // 2**19 * M0
    gf_mul(L2, L0, T0); // L0 + T0
    gf_sqr(L1, T0); // 2**20 * M0
    gf_sqr(T1, L1); // 2**21 * M0
    gf_mul(M1, M0, T1); // M0 + T1
    gf_sqrn(T0, M1, 40); // 2**40 * M1
    gf_mul(L0, L2, T0); // L2 + T0
    gf_sqr(L1, T0); // 2**41 * M1
    gf_sqr(T2, L1); // 2**42 * M1
    gf_mul(M0, M1, T2); // M1 + T2
    gf_sqrn(T0, M0, 82); // 2**82 * M0
    gf_mul(L1, L0, T0); // L0 + T0
    gf_sqr(L2, L1);
    gf_mul(L0, L2, x); // L2 + 1
    gf_copy(a, L1);
    
    return gf_eq(L0, ONE);
}
