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

#include <stdio.h>
#include <stdlib.h>

static void field_print2(const gf f) {
    unsigned char ser[X_SER_BYTES];
    int i;
    gf_serialize(ser,f,1);
    fprintf(stderr, "0x");
    for (i=X_SER_BYTES-1; i>=0; i--) {
        fprintf(stderr, "%02x", ser[i]);
    }
}

mask_t gf_isr (
    gf a,
    const gf x
) {
    gf L0, L1, L2;
    
    fprintf(stderr, "x: ");
    field_print2(x);
    fprintf(stderr, "\n");
    gf_sqr  (L1,     x );
    fprintf(stderr, "L1: ");
    field_print2(L1);
    fprintf(stderr, "\n");
    gf_mul  (L2,     x,   L1 );
    fprintf(stderr, "L2: ");
    field_print2(L2);
    fprintf(stderr, "\n");
    gf_sqr  (L1,   L2 );
    fprintf(stderr, "L1: ");
    field_print2(L1);
    fprintf(stderr, "\n");
    gf_mul  (L2,     x,   L1 );
    fprintf(stderr, "L2: ");
    field_print2(L2);
    fprintf(stderr, "\n");
    gf_sqrn (L1,   L2,     3 );
    fprintf(stderr, "L1: ");
    field_print2(L1);
    fprintf(stderr, "\n");
    gf_mul  (L0,   L2,   L1 );
    fprintf(stderr, "L0: ");
    field_print2(L0);
    fprintf(stderr, "\n");
    gf_sqrn (L1,   L0,     3 );
    fprintf(stderr, "L1: ");
    field_print2(L1);
    fprintf(stderr, "\n");
    gf_mul  (L0,   L2,   L1 );
    fprintf(stderr, "L0: ");
    field_print2(L0);
    fprintf(stderr, "\n");
    gf_sqrn (L2,   L0,     9 );
    fprintf(stderr, "L2: ");
    field_print2(L2);
    fprintf(stderr, "\n");
    gf_mul  (L1,   L0,   L2 );
    fprintf(stderr, "L1: ");
    field_print2(L1);
    fprintf(stderr, "\n");
    gf_sqr  (L0,   L1 );
    fprintf(stderr, "L0: ");
    field_print2(L0);
    fprintf(stderr, "\n");
    gf_mul  (L2,     x,   L0 );
    fprintf(stderr, "L2: ");
    field_print2(L2);
    fprintf(stderr, "\n");
    gf_sqrn (L0,   L2,    18 );
    fprintf(stderr, "L0: ");
    field_print2(L0);
    fprintf(stderr, "\n");
    gf_mul  (L2,   L1,   L0 );
    fprintf(stderr, "L2: ");
    field_print2(L2);
    fprintf(stderr, "\n");
    gf_sqrn (L0,   L2,    37 );
    fprintf(stderr, "L0: ");
    field_print2(L0);
    fprintf(stderr, "\n");
    gf_mul  (L1,   L2,   L0 );
    fprintf(stderr, "L1: ");
    field_print2(L1);
    fprintf(stderr, "\n");
    gf_sqrn (L0,   L1,    37 );
    fprintf(stderr, "L0: ");
    field_print2(L0);
    fprintf(stderr, "\n");
    gf_mul  (L1,   L2,   L0 );
    fprintf(stderr, "L1: ");
    field_print2(L1);
    fprintf(stderr, "\n");
    gf_sqrn (L0,   L1,   111 );
    fprintf(stderr, "L0: ");
    field_print2(L0);
    fprintf(stderr, "\n");
    gf_mul  (L2,   L1,   L0 );
    fprintf(stderr, "L2: ");
    field_print2(L2);
    fprintf(stderr, "\n");
    gf_sqr  (L0,   L2 );
    fprintf(stderr, "L0: ");
    field_print2(L0);
    fprintf(stderr, "\n");
    gf_mul  (L1,     x,   L0 );
    fprintf(stderr, "L1: ");
    field_print2(L1);
    fprintf(stderr, "\n");
    gf_sqrn (L0,   L1,   223 );
    fprintf(stderr, "L0: ");
    field_print2(L0);
    fprintf(stderr, "\n");
    gf_mul  (L1,   L2,   L0 );
    fprintf(stderr, "L1: ");
    field_print2(L1);
    fprintf(stderr, "\n");
    gf_sqr  (L2, L1);
    fprintf(stderr, "L2: ");
    field_print2(L2);
    fprintf(stderr, "\n");
    gf_mul  (L0, L2, x);
    fprintf(stderr, "L0: ");
    field_print2(L0);
    fprintf(stderr, "\n");
    gf_copy(a,L1);
    fprintf(stderr, "a: ");
    field_print2(a);
    fprintf(stderr, "\n");
    
    return gf_eq(L0,ONE);
}
