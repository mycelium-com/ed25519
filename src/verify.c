#include "ed25519.h"
#include "sha3.h"
#include "ge.h"
#include "sc.h"

inline static int myc_ed25519_consttime_equal_32(const unsigned char *x, const unsigned char *y) {
    unsigned char r = 0;

    r = x[0] ^ y[0];
    #define F(i) r |= x[i] ^ y[i]
    F(1);
    F(2);
    F(3);
    F(4);
    F(5);
    F(6);
    F(7);
    F(8);
    F(9);
    F(10);
    F(11);
    F(12);
    F(13);
    F(14);
    F(15);
    F(16);
    F(17);
    F(18);
    F(19);
    F(20);
    F(21);
    F(22);
    F(23);
    F(24);
    F(25);
    F(26);
    F(27);
    F(28);
    F(29);
    F(30);
    F(31);
    #undef F

    return !r;
}

int myc_ed25519_verify(const unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key) {
    unsigned char h[64];
    unsigned char checker[32];
    MYC_SHA3_CTX hash;
    myc_ge_p3 A;
    myc_ge_p2 R;

    if (signature[63] & 224) {
        return 0;
    }

    if (myc_ge_frombytes_negate_vartime(&A, public_key) != 0) {
        return 0;
    }

    myc_sha3_512_Init(&hash);
    myc_sha3_Update(&hash, signature, 32);
    myc_sha3_Update(&hash, public_key, 32);
    myc_sha3_Update(&hash, message, message_len);
    myc_sha3_Final(&hash, h);
    
    myc_sc_reduce(h);
    myc_ge_double_scalarmult_vartime(&R, h, &A, signature + 32);
    myc_ge_tobytes(checker, &R);

    if (!myc_ed25519_consttime_equal_32(checker, signature)) {
        return 0;
    }

    return 1;
}
