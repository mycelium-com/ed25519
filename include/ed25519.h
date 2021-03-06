#ifndef ED25519_H
#define ED25519_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void ed25519_create_privkey(unsigned char *private_key, const unsigned char *seed);
void ed25519_create_keypair(unsigned char *public_key, unsigned char *private_key, const unsigned char *seed);
void ed25519_get_pubkey(unsigned char *public_key, const unsigned char *private_key);
int ed25519_verify_pubkey(const unsigned char *public_key);
int ed25519_verify_master_hash(const unsigned char *seed_hash);
int ed25519_verify_privkey(const unsigned char *private_key);
int ed25519_sign(unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *private_key);
int ed25519_verify(const unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key);
int ed25519_add_scalar(unsigned char *public_key, unsigned char *private_key, const unsigned char *scalar);
void ed25519_key_exchange(unsigned char *shared_secret, const unsigned char *public_key, const unsigned char *private_key);

#ifdef __cplusplus
}
#endif

#endif
