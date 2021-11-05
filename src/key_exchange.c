#include "ed25519.h"
#include "fe.h"

void myc_ed25519_key_exchange(unsigned char *shared_secret, const unsigned char *public_key, const unsigned char *private_key) {
    unsigned char e[32];
    unsigned int i;
    
    myc_fe x1;
    myc_fe x2;
    myc_fe z2;
    myc_fe x3;
    myc_fe z3;
    myc_fe tmp0;
    myc_fe tmp1;

    int pos;
    unsigned int swap;
    unsigned int b;

    /* copy the private key and make sure it's valid */
    for (i = 0; i < 32; ++i) {
        e[i] = private_key[i];
    }

    e[0] &= 248;
    e[31] &= 63;
    e[31] |= 64;

    /* unpack the public key and convert edwards to montgomery */
    /* due to CodesInChaos: montgomeryX = (edwardsY + 1)*inverse(1 - edwardsY) mod p */
    myc_fe_frombytes(x1, public_key);
    myc_fe_1(tmp1);
    myc_fe_add(tmp0, x1, tmp1);
    myc_fe_sub(tmp1, tmp1, x1);
    myc_fe_invert(tmp1, tmp1);
    myc_fe_mul(x1, tmp0, tmp1);

    myc_fe_1(x2);
    myc_fe_0(z2);
    myc_fe_copy(x3, x1);
    myc_fe_1(z3);

    swap = 0;
    for (pos = 254; pos >= 0; --pos) {
        b = e[pos / 8] >> (pos & 7);
        b &= 1;
        swap ^= b;
        myc_fe_cswap(x2, x3, swap);
        myc_fe_cswap(z2, z3, swap);
        swap = b;

        /* from montgomery.h */
        myc_fe_sub(tmp0, x3, z3);
        myc_fe_sub(tmp1, x2, z2);
        myc_fe_add(x2, x2, z2);
        myc_fe_add(z2, x3, z3);
        myc_fe_mul(z3, tmp0, x2);
        myc_fe_mul(z2, z2, tmp1);
        myc_fe_sq(tmp0, tmp1);
        myc_fe_sq(tmp1, x2);
        myc_fe_add(x3, z3, z2);
        myc_fe_sub(z2, z3, z2);
        myc_fe_mul(x2, tmp1, tmp0);
        myc_fe_sub(tmp1, tmp1, tmp0);
        myc_fe_sq(z2, z2);
        myc_fe_mul121666(z3, tmp1);
        myc_fe_sq(x3, x3);
        myc_fe_add(tmp0, tmp0, z3);
        myc_fe_mul(z3, x1, z2);
        myc_fe_mul(z2, tmp1, tmp0);
    }

    myc_fe_cswap(x2, x3, swap);
    myc_fe_cswap(z2, z3, swap);

    myc_fe_invert(z2, z2);
    myc_fe_mul(x2, x2, z2);
    myc_fe_tobytes(shared_secret, x2);
}
