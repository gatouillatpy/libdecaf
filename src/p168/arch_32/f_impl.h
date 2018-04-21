/* Copyright (c) 2014-2016 Cryptography Research, Inc.
 * Released under the MIT License.  See LICENSE.txt for license information.
 */

#define GF_HEADROOM 2
#define LIMB(x) (x##ull)&((1ull<<21)-1), (x##ull)>>21
#define FIELD_LITERAL(a,b,c,d) \
    {{LIMB(a),LIMB(b),LIMB(c),LIMB(d)}}
    
#define LIMB_PLACE_VALUE(i) 21

void gf_add_RAW (gf out, const gf a, const gf b) {
    for (unsigned int i=0; i<8; i++) {
        out->limb[i] = a->limb[i] + b->limb[i];
    }
}

void gf_sub_RAW (gf out, const gf a, const gf b) {
    for (unsigned int i=0; i<8; i++) {
        out->limb[i] = a->limb[i] - b->limb[i];
    }
}

void gf_bias (gf a, int amt) {    
    uint32_t coe = ((1ull<<21)-1)*amt, co0 = coe-256*amt;
    for (unsigned int i=0; i<8; i++) {
        a->limb[i] += ((i==0) ? co0 : coe);
    }
}

void gf_weak_reduce (gf a) {
    uint32_t mask = (1ull<<21) - 1;
    uint32_t tmp = a->limb[7] >> 21;
    for (unsigned int i=7; i>0; i--) {
        a->limb[i] = (a->limb[i] & mask) + (a->limb[i-1]>>21);
    }
    a->limb[0] = (a->limb[0] & mask) + tmp*257;
}

