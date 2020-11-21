#include "ed25519.h"
#include "sha3.h"
#include "ge.h"
#include <string.h>
/**
  *  Return '1' if the bit value at position y within x is '1' and '0' if it's 0  by ANDing x with
  *  a bit mask where the bit in y's position is '1' and '0' elsewhere and comparing it to all 0's.
  *
  *  Returns '1' in least significant bit position if the value of the bit is '1', '0' if it was '0'.
  */
inline static unsigned int read_bit(unsigned int x, unsigned int y) {
    return ((0u == (x & (1u << y))) ? 0u : 1u);
}

void ed25519_get_pubkey(unsigned char *public_key, const unsigned char *private_key) {
    ge_p3 A;

    ge_scalarmult_base(&A, private_key);
    ge_p3_tobytes(public_key, &A);
}

int ed25519_verify_privkey(const unsigned char *private_key) {
    unsigned char key_copy[32];
    if (0 != read_bit(private_key[0], 5)) {
        memcpy(key_copy, private_key, 32);
        key_copy[0] &= 248u;
        key_copy[31] &= 127u;
        key_copy[31] |= 64u;
        return memcmp(key_copy, private_key, 32) == 0;
    }
    return 0;
}

int ed25519_verify_pubkey(const unsigned char *public_key) {
    ge_p3 A;
    if (ge_frombytes_negate_vartime(&A, public_key) != 0) {
        return 0;
    }
    return 1;
}

void ed25519_create_privkey(unsigned char *private_key, const unsigned char *seed) {
    sha3_256(seed, 32, private_key);
    private_key[0] &= 248;
    private_key[31] &= 63;
    private_key[31] |= 64;
}

void ed25519_create_keypair(unsigned char *public_key, unsigned char *private_key, const unsigned char *seed) {
    sha3_256(seed, 32, private_key);
    private_key[0] &= 248;
    private_key[31] &= 63;
    private_key[31] |= 64;
    ed25519_get_pubkey(public_key, private_key);
}
