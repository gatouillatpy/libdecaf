/**
 * @file curve25519/eddsa.c
 * @author Mike Hamburg
 *
 * @copyright
 *   Copyright (c) 2015-2016 Cryptography Research, Inc.  \n
 *   Released under the MIT License.  See LICENSE.txt for license information.
 *
 * @cond internal
 * @brief EdDSA routines.
 *
 * @warning This file was automatically generated in Python.
 * Please do not edit it.
 */
#include "word.h"
#include "field.h"
#include <decaf.h>
#include <decaf/ed255.h>
#include <decaf/shake.h>
#include <decaf/sha512.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <decaf/point_255.h>

#define API_NAME "decaf_255"
#define API_NS(_id) decaf_255_##_id
#define scalar_t API_NS(scalar_t)
#define SCALAR_LIMBS DECAF_255_SCALAR_LIMBS

static void field_print2(const gf f) {
    unsigned char ser[X_SER_BYTES];
    int i;
    gf_serialize(ser,f,1);
    fprintf(stderr, "0x");
    for (i=X_SER_BYTES-1; i>=0; i--) {
        fprintf(stderr, "%02x", ser[i]);
    }
}

/*static void field_print2_raw(const gf f) {
    unsigned char ser[2*X_SER_BYTES];
    int i; int k = 0;
    for (i=0; i<SCALAR_LIMBS; i++) {
        decaf_word_t x = f->limb[i];
        ser[k++] = x & 0xFF;
        ser[k++] = (x >> 8) & 0xFF;
        ser[k++] = (x >> 16) & 0xFF;
        ser[k++] = (x >> 24) & 0xFF;
    }
    fprintf(stderr, "0x");
    for (i=X_SER_BYTES-1; i>=0; i--) {
        fprintf(stderr, "%02x", ser[i]);
    }
}*/

static void field_print3(const uint8_t *ser) {
    int i;
    fprintf(stderr, "0x");
    for (i = DECAF_EDDSA_25519_PRIVATE_BYTES - 1; i >= 0; i--) {
        fprintf(stderr, "%02x", ser[i]);
    }
}

static void field_print4(const scalar_t f) {
    unsigned char ser[2*X_SER_BYTES];
    int i; int k = 0;
    for (i=0; i<SCALAR_LIMBS; i++) {
        decaf_word_t x = f->limb[i];
        ser[k++] = x & 0xFF;
        ser[k++] = (x >> 8) & 0xFF;
        ser[k++] = (x >> 16) & 0xFF;
        ser[k++] = (x >> 24) & 0xFF;
    }
    fprintf(stderr, "0x");
    for (i=X_SER_BYTES-1; i>=0; i--) {
        fprintf(stderr, "%02x", ser[i]);
    }
}

#define hash_ctx_t   decaf_sha512_ctx_t
#define hash_init    decaf_sha512_init
#define hash_update  decaf_sha512_update
#define hash_final   decaf_sha512_final
#define hash_destroy decaf_sha512_destroy
#define hash_hash    decaf_sha512_hash

#define NO_CONTEXT DECAF_EDDSA_25519_SUPPORTS_CONTEXTLESS_SIGS
#define EDDSA_USE_SIGMA_ISOGENY 1
#define COFACTOR 8
#define EDDSA_PREHASH_BYTES 64

#if NO_CONTEXT
const uint8_t DECAF_ED25519_NO_CONTEXT_POINTS_HERE = 0;
const uint8_t * const DECAF_ED25519_NO_CONTEXT = &DECAF_ED25519_NO_CONTEXT_POINTS_HERE;
#endif

static void clamp (
    uint8_t secret_scalar_ser[DECAF_EDDSA_25519_PRIVATE_BYTES]
) {
    /* Blarg */
    secret_scalar_ser[0] &= -COFACTOR;
    uint8_t hibit = (1<<7)>>1;
    if (hibit == 0) {
        secret_scalar_ser[DECAF_EDDSA_25519_PRIVATE_BYTES - 1] = 0;
        secret_scalar_ser[DECAF_EDDSA_25519_PRIVATE_BYTES - 2] |= 0x80;
    } else {
        secret_scalar_ser[DECAF_EDDSA_25519_PRIVATE_BYTES - 1] &= hibit-1;
        secret_scalar_ser[DECAF_EDDSA_25519_PRIVATE_BYTES - 1] |= hibit;
    }
}

static void hash_init_with_dom(
    hash_ctx_t hash,
    uint8_t prehashed,
    uint8_t for_prehash,
    const uint8_t *context,
    uint8_t context_len
) {
    hash_init(hash);

#if NO_CONTEXT
    if (context_len == 0 && context == DECAF_ED25519_NO_CONTEXT) {
        (void)prehashed;
        (void)for_prehash;
        (void)context;
        (void)context_len;
        return;
    }
#endif
    const char *dom_s = "SigEd25519 no Ed25519 collisions";
    const uint8_t dom[2] = {2+word_is_zero(prehashed)+word_is_zero(for_prehash), context_len};
    hash_update(hash,(const unsigned char *)dom_s, strlen(dom_s));
    hash_update(hash,dom,2);
    hash_update(hash,context,context_len);
}

void decaf_ed25519_prehash_init (
    hash_ctx_t hash
) {
    hash_init(hash);
}

/* In this file because it uses the hash */
void decaf_ed25519_convert_private_key_to_x25519 (
    uint8_t x[DECAF_X25519_PRIVATE_BYTES],
    const uint8_t ed[DECAF_EDDSA_25519_PRIVATE_BYTES]
) {
    /* pass the private key through hash_hash function */
    /* and keep the first DECAF_X25519_PRIVATE_BYTES bytes */
    hash_hash(
        x,
        DECAF_X25519_PRIVATE_BYTES,
        ed,
        DECAF_EDDSA_25519_PRIVATE_BYTES
    );
}
    
void decaf_ed25519_derive_public_key (
    uint8_t pubkey[DECAF_EDDSA_25519_PUBLIC_BYTES],
    const uint8_t privkey[DECAF_EDDSA_25519_PRIVATE_BYTES]
) {
    int i;
    /* only this much used for keygen */
    uint8_t secret_scalar_ser[DECAF_EDDSA_25519_PRIVATE_BYTES];
    
    hash_hash(
        secret_scalar_ser,
        sizeof(secret_scalar_ser),
        privkey,
        DECAF_EDDSA_25519_PRIVATE_BYTES
    );
    clamp(secret_scalar_ser);
    fprintf(stderr, "secret_scalar_ser="); field_print3(secret_scalar_ser); fprintf(stderr, "\n");
        
    API_NS(scalar_t) secret_scalar;
    API_NS(scalar_decode_long)(secret_scalar, secret_scalar_ser, sizeof(secret_scalar_ser));
    fprintf(stderr, "secret_scalar="); field_print4(secret_scalar); fprintf(stderr, "\n");
    
    /* Since we are going to mul_by_cofactor during encoding, divide by it here.
     * However, the EdDSA base point is not the same as the decaf base point if
     * the sigma isogeny is in use: the EdDSA base point is on Etwist_d/(1-d) and
     * the decaf base point is on Etwist_d, and when converted it effectively
     * picks up a factor of 2 from the isogenies.  So we might start at 2 instead of 1. 
     */
    for (unsigned int c=1; c<DECAF_255_EDDSA_ENCODE_RATIO; c <<= 1) {
        API_NS(scalar_halve)(secret_scalar,secret_scalar);
    }
    fprintf(stderr, "secret_scalar="); field_print4(secret_scalar); fprintf(stderr, " (after ratio division)\n");

    fprintf(stderr, "point_base->x="); field_print2(API_NS(point_base)->x); fprintf(stderr, "\n");
    fprintf(stderr, "point_base->y="); field_print2(API_NS(point_base)->y); fprintf(stderr, "\n");
    fprintf(stderr, "point_base->z="); field_print2(API_NS(point_base)->z); fprintf(stderr, "\n");
    fprintf(stderr, "point_base->t="); field_print2(API_NS(point_base)->t); fprintf(stderr, "\n");

    API_NS(point_t) p;
    //API_NS(precomputed_scalarmul)(p,API_NS(precomputed_base),secret_scalar);
    API_NS(point_scalarmul)(p,API_NS(point_base),secret_scalar); // TODO: put the one above back
    fprintf(stderr, "p->x="); field_print2(p->x); fprintf(stderr, "\n");
    fprintf(stderr, "p->y="); field_print2(p->y); fprintf(stderr, "\n");
    fprintf(stderr, "p->z="); field_print2(p->z); fprintf(stderr, "\n");
    fprintf(stderr, "p->t="); field_print2(p->t); fprintf(stderr, "\n");
    
    // TEMP
    API_NS(scalar_t) s;
    for (i=0; i<SCALAR_LIMBS; i++) {
        s->limb[i] = 0;
    }
    s->limb[0] = 1;
    fprintf(stderr, "s="); field_print4(s); fprintf(stderr, "\n");
    for (unsigned int c=1; c<DECAF_255_EDDSA_ENCODE_RATIO; c <<= 1) {
        API_NS(scalar_halve)(s,s);
    }
    fprintf(stderr, "s="); field_print4(s); fprintf(stderr, " (after ratio division)\n");
    API_NS(point_t) q;
    API_NS(point_scalarmul)(q,API_NS(point_base),s);
    fprintf(stderr, "q->x="); field_print2(q->x); fprintf(stderr, "\n");
    fprintf(stderr, "q->y="); field_print2(q->y); fprintf(stderr, "\n");
    fprintf(stderr, "q->z="); field_print2(q->z); fprintf(stderr, "\n");
    fprintf(stderr, "q->t="); field_print2(q->t); fprintf(stderr, "\n");
    API_NS(point_mul_by_ratio_and_encode_like_eddsa)(pubkey, q);
    fprintf(stderr, "e="); field_print3(pubkey); fprintf(stderr, "\n");
    API_NS(scalar_destroy)(s);
    API_NS(point_destroy)(q);
    // END TEMP
    
    // TEST
    fprintf(stderr, "TEST point_scalarmul\n");
    API_NS(scalar_t) s_mul;
    for (i=0; i<SCALAR_LIMBS; i++) {
        s_mul->limb[i] = 0;
    }
    s_mul->limb[0] = 2;
    fprintf(stderr, "s_mul="); field_print4(s_mul); fprintf(stderr, "\n");
    API_NS(point_t) p_mul;
    API_NS(point_scalarmul)(p_mul,API_NS(point_base),s_mul);
    fprintf(stderr, "p_mul->x="); field_print2(p_mul->x); fprintf(stderr, "\n");
    fprintf(stderr, "p_mul->y="); field_print2(p_mul->y); fprintf(stderr, "\n");
    fprintf(stderr, "p_mul->z="); field_print2(p_mul->z); fprintf(stderr, "\n");
    fprintf(stderr, "p_mul->t="); field_print2(p_mul->t); fprintf(stderr, "\n");
    API_NS(scalar_destroy)(s_mul);
    API_NS(point_destroy)(p_mul);
    fprintf(stderr, "TEST point_add\n");
    API_NS(point_t) p_add;
    API_NS(point_add)(p_add,API_NS(point_base),API_NS(point_base));
    fprintf(stderr, "p_add->x="); field_print2(p_add->x); fprintf(stderr, "\n");
    fprintf(stderr, "p_add->y="); field_print2(p_add->y); fprintf(stderr, "\n");
    fprintf(stderr, "p_add->z="); field_print2(p_add->z); fprintf(stderr, "\n");
    fprintf(stderr, "p_add->t="); field_print2(p_add->t); fprintf(stderr, "\n");
    API_NS(point_destroy)(p_add);
    fprintf(stderr, "TEST point_double\n");
    API_NS(point_t) p_double;
    API_NS(point_double)(p_double,API_NS(point_base));
    fprintf(stderr, "p_double->x="); field_print2(p_double->x); fprintf(stderr, "\n");
    fprintf(stderr, "p_double->y="); field_print2(p_double->y); fprintf(stderr, "\n");
    fprintf(stderr, "p_double->z="); field_print2(p_double->z); fprintf(stderr, "\n");
    fprintf(stderr, "p_double->t="); field_print2(p_double->t); fprintf(stderr, "\n");
    API_NS(point_destroy)(p_double);
    decaf_bool_t b0 = API_NS(point_eq)(p_mul, p_add);
    fprintf(stderr, "b0=%d\n", (int)b0);
    decaf_bool_t b1 = API_NS(point_eq)(p_mul, p_double);
    fprintf(stderr, "b1=%d\n", (int)b1);
    decaf_bool_t b2 = API_NS(point_eq)(p_add, p_double);
    fprintf(stderr, "b2=%d\n\n", (int)b2);
    // END TEST
    
    API_NS(point_mul_by_ratio_and_encode_like_eddsa)(pubkey, p);
        
    /* Cleanup */
    API_NS(scalar_destroy)(secret_scalar);
    API_NS(point_destroy)(p);
    decaf_bzero(secret_scalar_ser, sizeof(secret_scalar_ser));
}

#ifdef _MSC_VER
#pragma pack(push,1)
typedef struct {
    uint8_t secret_scalar_ser[DECAF_EDDSA_25519_PRIVATE_BYTES];
    uint8_t seed[DECAF_EDDSA_25519_PRIVATE_BYTES];
} expanded_t;
#pragma pack(pop)
#endif

void decaf_ed25519_sign (
    uint8_t signature[DECAF_EDDSA_25519_SIGNATURE_BYTES],
    const uint8_t privkey[DECAF_EDDSA_25519_PRIVATE_BYTES],
    const uint8_t pubkey[DECAF_EDDSA_25519_PUBLIC_BYTES],
    const uint8_t *message,
    size_t message_len,
    uint8_t prehashed,
    const uint8_t *context,
    uint8_t context_len
) {
    API_NS(scalar_t) secret_scalar;
    hash_ctx_t hash;
    {
        /* Schedule the secret key */
#ifdef _MSC_VER
        expanded_t expanded;
#else
        struct {
            uint8_t secret_scalar_ser[DECAF_EDDSA_25519_PRIVATE_BYTES];
            uint8_t seed[DECAF_EDDSA_25519_PRIVATE_BYTES];
        } __attribute__((packed)) expanded;
#endif
        hash_hash(
            (uint8_t *)&expanded,
            sizeof(expanded),
            privkey,
            DECAF_EDDSA_25519_PRIVATE_BYTES
        );
        clamp(expanded.secret_scalar_ser);
        API_NS(scalar_decode_long)(secret_scalar, expanded.secret_scalar_ser, sizeof(expanded.secret_scalar_ser));
    
        /* Hash to create the nonce */
        hash_init_with_dom(hash,prehashed,0,context,context_len);
        hash_update(hash,expanded.seed,sizeof(expanded.seed));
        hash_update(hash,message,message_len);
        decaf_bzero(&expanded, sizeof(expanded));
    }
    
    /* Decode the nonce */
    API_NS(scalar_t) nonce_scalar;
    {
        uint8_t nonce[2*DECAF_EDDSA_25519_PRIVATE_BYTES];
        hash_final(hash,nonce,sizeof(nonce));
        API_NS(scalar_decode_long)(nonce_scalar, nonce, sizeof(nonce));
        decaf_bzero(nonce, sizeof(nonce));
    }
    
    uint8_t nonce_point[DECAF_EDDSA_25519_PUBLIC_BYTES] = {0};
    {
        /* Scalarmul to create the nonce-point */
        API_NS(scalar_t) nonce_scalar_2;
        API_NS(scalar_halve)(nonce_scalar_2,nonce_scalar);
        for (unsigned int c = 2; c < DECAF_255_EDDSA_ENCODE_RATIO; c <<= 1) {
            API_NS(scalar_halve)(nonce_scalar_2,nonce_scalar_2);
        }
        
        API_NS(point_t) p;
        API_NS(precomputed_scalarmul)(p,API_NS(precomputed_base),nonce_scalar_2);
        API_NS(point_mul_by_ratio_and_encode_like_eddsa)(nonce_point, p);
        API_NS(point_destroy)(p);
        API_NS(scalar_destroy)(nonce_scalar_2);
    }
    
    API_NS(scalar_t) challenge_scalar;
    {
        /* Compute the challenge */
        hash_init_with_dom(hash,prehashed,0,context,context_len);
        hash_update(hash,nonce_point,sizeof(nonce_point));
        hash_update(hash,pubkey,DECAF_EDDSA_25519_PUBLIC_BYTES);
        hash_update(hash,message,message_len);
        uint8_t challenge[2*DECAF_EDDSA_25519_PRIVATE_BYTES];
        hash_final(hash,challenge,sizeof(challenge));
        hash_destroy(hash);
        API_NS(scalar_decode_long)(challenge_scalar,challenge,sizeof(challenge));
        decaf_bzero(challenge,sizeof(challenge));
    }
    
    API_NS(scalar_mul)(challenge_scalar,challenge_scalar,secret_scalar);
    API_NS(scalar_add)(challenge_scalar,challenge_scalar,nonce_scalar);
    
    decaf_bzero(signature,DECAF_EDDSA_25519_SIGNATURE_BYTES);
    memcpy(signature,nonce_point,sizeof(nonce_point));
    API_NS(scalar_encode)(&signature[DECAF_EDDSA_25519_PUBLIC_BYTES],challenge_scalar);
    
    API_NS(scalar_destroy)(secret_scalar);
    API_NS(scalar_destroy)(nonce_scalar);
    API_NS(scalar_destroy)(challenge_scalar);
}


void decaf_ed25519_sign_prehash (
    uint8_t signature[DECAF_EDDSA_25519_SIGNATURE_BYTES],
    const uint8_t privkey[DECAF_EDDSA_25519_PRIVATE_BYTES],
    const uint8_t pubkey[DECAF_EDDSA_25519_PUBLIC_BYTES],
    const decaf_ed25519_prehash_ctx_t hash,
    const uint8_t *context,
    uint8_t context_len
) {
    uint8_t hash_output[EDDSA_PREHASH_BYTES];
    {
        decaf_ed25519_prehash_ctx_t hash_too;
        memcpy(hash_too,hash,sizeof(hash_too));
        hash_final(hash_too,hash_output,sizeof(hash_output));
        hash_destroy(hash_too);
    }

    decaf_ed25519_sign(signature,privkey,pubkey,hash_output,sizeof(hash_output),1,context,context_len);
    decaf_bzero(hash_output,sizeof(hash_output));
}

decaf_error_t decaf_ed25519_verify (
    const uint8_t signature[DECAF_EDDSA_25519_SIGNATURE_BYTES],
    const uint8_t pubkey[DECAF_EDDSA_25519_PUBLIC_BYTES],
    const uint8_t *message,
    size_t message_len,
    uint8_t prehashed,
    const uint8_t *context,
    uint8_t context_len
) { 
    API_NS(point_t) pk_point, r_point;
    decaf_error_t error = API_NS(point_decode_like_eddsa_and_mul_by_ratio)(pk_point,pubkey);
    if (DECAF_SUCCESS != error) { return error; }
    
    error = API_NS(point_decode_like_eddsa_and_mul_by_ratio)(r_point,signature);
    if (DECAF_SUCCESS != error) { return error; }
    
    API_NS(scalar_t) challenge_scalar;
    {
        /* Compute the challenge */
        hash_ctx_t hash;
        hash_init_with_dom(hash,prehashed,0,context,context_len);
        hash_update(hash,signature,DECAF_EDDSA_25519_PUBLIC_BYTES);
        hash_update(hash,pubkey,DECAF_EDDSA_25519_PUBLIC_BYTES);
        hash_update(hash,message,message_len);
        uint8_t challenge[2*DECAF_EDDSA_25519_PRIVATE_BYTES];
        hash_final(hash,challenge,sizeof(challenge));
        hash_destroy(hash);
        API_NS(scalar_decode_long)(challenge_scalar,challenge,sizeof(challenge));
        decaf_bzero(challenge,sizeof(challenge));
    }
    API_NS(scalar_sub)(challenge_scalar, API_NS(scalar_zero), challenge_scalar);
    
    API_NS(scalar_t) response_scalar;
    API_NS(scalar_decode_long)(
        response_scalar,
        &signature[DECAF_EDDSA_25519_PUBLIC_BYTES],
        DECAF_EDDSA_25519_PRIVATE_BYTES
    );
    
    for (unsigned c=1; c<DECAF_255_EDDSA_DECODE_RATIO; c<<=1) {
        API_NS(scalar_add)(response_scalar,response_scalar,response_scalar);
    }
    
    
    /* pk_point = -c(x(P)) + (cx + k)G = kG */
    API_NS(base_double_scalarmul_non_secret)(
        pk_point,
        response_scalar,
        pk_point,
        challenge_scalar
    );
    return decaf_succeed_if(API_NS(point_eq(pk_point,r_point)));
}


decaf_error_t decaf_ed25519_verify_prehash (
    const uint8_t signature[DECAF_EDDSA_25519_SIGNATURE_BYTES],
    const uint8_t pubkey[DECAF_EDDSA_25519_PUBLIC_BYTES],
    const decaf_ed25519_prehash_ctx_t hash,
    const uint8_t *context,
    uint8_t context_len
) {
    decaf_error_t ret;
    
    uint8_t hash_output[EDDSA_PREHASH_BYTES];
    {
        decaf_ed25519_prehash_ctx_t hash_too;
        memcpy(hash_too,hash,sizeof(hash_too));
        hash_final(hash_too,hash_output,sizeof(hash_output));
        hash_destroy(hash_too);
    }
    
    ret = decaf_ed25519_verify(signature,pubkey,hash_output,sizeof(hash_output),1,context,context_len);
    
    return ret;
}
