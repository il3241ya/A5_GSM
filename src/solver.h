#ifndef A5_SOLVER_H
#define A5_SOLVER_H

#include <stddef.h>

/* key length in bits */
#define A5_KEY_LEN 64
/* frame length in bit */
#define A5_FRAME_LEN 22
/* number of dry iterations before the ciphering begins, might be useful to the caller */
#define A5_DRY_ITERS_COUNT 100
/* length of single block of opentext & ciphertext */
#define A5_CHUNK_LEN 114

/* equals max error code + 1 */
#define A5_MAX_ERROR_CODE 3

#ifdef __cplusplus

#define A5_DECL_BEGIN extern "C" {
#define A5_DECL_END }

#else

#define A5_DECL_BEGIN
#define A5_DECL_END

#endif

A5_DECL_BEGIN

enum a5_error {
    A5_ERR_OK = 0,
    A5_ERR_FRAME_INVALID = 1,
    A5_ERR_NOMEM
};

/* arbitrary structure that holds state of cypher */
struct a5_cipher_state;

/* generate new state for cypher */
struct a5_cipher_state* a5_new_cipher_state();

/* write key to state, returns 0 on success */
int a5_write_key(struct a5_cipher_state* state, const char* key);

/* write frame number to state, returns 0 on success
   note, it is the caller's responsibility to ensure that key
   is written first */
int a5_write_frame(struct a5_cipher_state* state, size_t frame);

/* run iteration for cypher, generating a single bit, returns 0 on success */
int a5_iter(struct a5_cipher_state* state, int* bit);

/* convenience function, inits state and runs full iteration on data, returns 0 on success */
int a5_cipher(
    struct a5_cipher_state* state,  /* state to operate on */
    const char* key,                /* key, should be A5_KEY_LEN long */
    size_t frame,                   /* frame, should be A5_FRAME_LEN long */
    const char* opentext,           /* opentext, should be A5_CHUNK_LEN long */
    char* ciphertext                /* output for cipher, should be as long as opentext */
);

/* guess what this does */
int a5_decipher(
    struct a5_cipher_state* state, 
    const char* key, 
    size_t frame, 
    const char* ciphertext, 
    char* opentext
);

/* reset state */
void a5_reset_state(struct a5_cipher_state* state);

/* free memory occupied by cypher state */
void a5_free_cipher_state(struct a5_cipher_state* state);

/* gets string for error code */
const char* a5_error_string(enum a5_error error);

A5_DECL_END

#endif 