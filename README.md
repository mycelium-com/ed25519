# ED25519 utilities library

This library is focused on providing ED25519 signing and verification.

### API

```c
/*
 * Derive private key from 256 bits of private seed.
 */
void myc_ed25519_create_privkey(unsigned char *private_key, const unsigned char *seed);
```

```c
/*
 * Derive private and public key from 256 bits of private seed.
 */
 void myc_ed25519_create_keypair(unsigned char *public_key, unsigned char *private_key, const unsigned char *seed);
 ```
 
 ```c
 /*
  * Get ED25519 public key from private key.
  */
  void myc_ed25519_get_pubkey(unsigned char *public_key, const unsigned char *private_key);
  
  ```c
  /*
   * Sign message using key pair.
   */
  void myc_ed25519_sign(unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key, const unsigned char *private_key);
  ```
  
  ```c
  /*
   * Verify ED25519 signature
   */
  int myc_ed25519_verify(const unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key);
  ```
  
  ```c
  /*
   * Tweak public or private key using the given scalar.
   */
  int myc_ed25519_add_scalar(unsigned char *public_key, unsigned char *private_key, const unsigned char *scalar);
  ```
  
  ```c
  /*
   * ECDH shared key derivation
   */
  void myc_ed25519_key_exchange(unsigned char *shared_secret, const unsigned char *public_key, const unsigned char *private_key);
  ```
 
