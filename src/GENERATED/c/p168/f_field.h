/**
 * @file p168/f_field.h
 * @author Mike Hamburg
 *
 * @copyright
 *   Copyright (c) 2015-2016 Cryptography Research, Inc.  \n
 *   Released under the MIT License.  See LICENSE.txt for license information.
 *
 * @brief Field-specific code for 2^168 - 2^8 - 1.
 *
 * @warning This file was automatically generated in Python.
 * Please do not edit it.
 */

#ifndef __P168_F_FIELD_H__
#define __P168_F_FIELD_H__ 1

#include "constant_time.h"
#include <string.h>
#include <assert.h>

#include "word.h"

#define __DECAF_168_GF_DEFINED__ 1
#define NLIMBS (32/sizeof(word_t))
#define X_SER_BYTES 21
#define SER_BYTES 21
typedef struct gf_168_s {
    word_t limb[NLIMBS];
#ifdef _MSC_VER
} /*__declspec(align(32))*/ gf_168_s, gf_168_t[1];
#else
} __attribute__((aligned(32))) gf_168_s, gf_168_t[1];
#endif

#define GF_LIT_LIMB_BITS  42
#define GF_BITS           168
#define ZERO              gf_168_ZERO
#define ONE               gf_168_ONE
#define MODULUS           gf_168_MODULUS
#define gf                gf_168_t
#define gf_s              gf_168_s
#define gf_eq             gf_168_eq
#define gf_hibit          gf_168_hibit
#define gf_lobit          gf_168_lobit
#define gf_copy           gf_168_copy
#define gf_add            gf_168_add
#define gf_sub            gf_168_sub
#define gf_add_RAW        gf_168_add_RAW
#define gf_sub_RAW        gf_168_sub_RAW
#define gf_bias           gf_168_bias
#define gf_weak_reduce    gf_168_weak_reduce
#define gf_strong_reduce  gf_168_strong_reduce
#define gf_mul            gf_168_mul
#define gf_sqr            gf_168_sqr
#define gf_mulw_unsigned  gf_168_mulw_unsigned
#define gf_isr            gf_168_isr
#define gf_serialize      gf_168_serialize
#define gf_deserialize    gf_168_deserialize

/* RFC 7748 support */
#define X_PUBLIC_BYTES  X_SER_BYTES
#define X_PRIVATE_BYTES X_PUBLIC_BYTES
#define X_PRIVATE_BITS  168

#define SQRT_MINUS_ONE    P168_SQRT_MINUS_ONE /* might not be defined */

#ifdef _MSC_VER
#define INLINE_UNUSED inline
#else
#define INLINE_UNUSED __inline__ __attribute__((unused,always_inline))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Defined below in f_impl.h */
static INLINE_UNUSED void gf_copy (gf out, const gf a) { *out = *a; }
static INLINE_UNUSED void gf_add_RAW (gf out, const gf a, const gf b);
static INLINE_UNUSED void gf_sub_RAW (gf out, const gf a, const gf b);
static INLINE_UNUSED void gf_bias (gf inout, int amount);
static INLINE_UNUSED void gf_weak_reduce (gf inout);

void gf_strong_reduce (gf inout);   
void gf_add (gf out, const gf a, const gf b);
void gf_sub (gf out, const gf a, const gf b);
void gf_mul (gf_s* DECAF_RESTRICT out, const gf a, const gf b);
void gf_mulw_unsigned (gf_s* DECAF_RESTRICT out, const gf a, uint32_t b);
void gf_sqr (gf_s* DECAF_RESTRICT out, const gf a);
mask_t gf_isr(gf a, const gf x); /** a^2 x = 1, QNR, or 0 if x=0.  Return true if successful */
mask_t gf_eq (const gf x, const gf y);
mask_t gf_lobit (const gf x);
mask_t gf_hibit (const gf x);

void gf_serialize (uint8_t *serial, const gf x,int with_highbit);
mask_t gf_deserialize (gf x, const uint8_t serial[SER_BYTES],int with_hibit,uint8_t hi_nmask);


#ifdef __cplusplus
} /* extern "C" */
#endif

#include "f_impl.h" /* Bring in the inline implementations */

#define P_MOD_8 7
#if P_MOD_8 == 5
    extern const gf SQRT_MINUS_ONE;
#endif

#ifndef LIMBPERM
  #define LIMBPERM(i) (i)
#endif
#define LIMB_MASK(i) (((1ull)<<LIMB_PLACE_VALUE(i))-1)

static const gf ZERO = {{{0}}}, ONE = {{{ [LIMBPERM(0)] = 1 }}};

#endif /* __P168_F_FIELD_H__ */
