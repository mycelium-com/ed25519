#include "ed25519.h"
#include "sha3.h"
#include "hmac_sha3.h"
#include "ge.h"
#include "sc.h"

int ed25519_sign(unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *private_key) {
    SHA3_CTX hash;
    unsigned char public_key[32];
    unsigned char hram[64];
    unsigned char r[64];
    ge_p3 R;

    // Public key is required for signing
    ed25519_get_pubkey(public_key, private_key);

    // Use hmac-sha3-512 to generate r parameter for signing
    hmac_sha3_512(private_key, 32, message, message_len, r, sizeof(r));

    sc_reduce(r);
    ge_scalarmult_base(&R, r);
    ge_p3_tobytes(signature, &R);

    sha3_512_Init(&hash);
    sha3_Update(&hash, signature, 32);
    sha3_Update(&hash, public_key, 32);
    sha3_Update(&hash, message, message_len);
    sha3_Final(&hash, hram);

    sc_reduce(hram);
    sc_muladd(signature + 32, hram, private_key, r);

    return 1;
}
