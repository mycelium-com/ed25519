#ifndef ED25519_H
#define ED25519_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

inline int ed25519_consttime_equal_4(const unsigned char *x, const unsigned char *y) {
    unsigned char r = 0;

    r = x[0] ^ y[0];
    #define F(i) r |= x[i] ^ y[i]
    F(1);
    F(2);
    F(3);
    #undef F

    return !r;
}

void ed25519_create_privkey(unsigned char *private_key, const unsigned char *seed);
void ed25519_create_keypair(unsigned char *public_key, unsigned char *private_key, const unsigned char *seed);
void ed25519_get_pubkey(unsigned char *public_key, const unsigned char *private_key);
void ed25519_sign(unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key, const unsigned char *private_key);
int ed25519_verify(const unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key);
int ed25519_add_scalar(unsigned char *public_key, unsigned char *private_key, const unsigned char *scalar);
void ed25519_key_exchange(unsigned char *shared_secret, const unsigned char *public_key, const unsigned char *private_key);

#ifdef __cplusplus
}
#endif

#endif
