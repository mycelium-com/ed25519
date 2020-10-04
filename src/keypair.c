#include "ed25519.h"
#include "sha3.h"
#include "ge.h"
#include <string.h>

void ed25519_get_pubkey(unsigned char *public_key, const unsigned char *private_key) {
    ge_p3 A;

    ge_scalarmult_base(&A, private_key);
    ge_p3_tobytes(public_key, &A);
}

int ed25519_verify_privkey(const unsigned char *private_key) {
    unsigned char key_copy[32];
    memcpy(key_copy, private_key, 32);
    key_copy[0] &= 248;
    key_copy[31] &= 127;
    key_copy[31] |= 64;
    return memcmp(key_copy, private_key, 32) == 0;
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
