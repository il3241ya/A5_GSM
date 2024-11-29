#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "solver.h"

#define A5_REG1_LEN 19
#define A5_REG2_LEN 22
#define A5_REG3_LEN 23

#define A5_REG1_CLOCKING_BIT_POS 8
#define A5_REG2_CLOCKING_BIT_POS 10
#define A5_REG3_CLOCKING_BIT_POS 10

#define A5_REG1_MASK 0x007200
#define A5_REG2_MASK 0x300000
#define A5_REG3_MASK 0x700100

const char* const errors[A5_MAX_ERROR_CODE] = {
    [A5_ERR_OK] = "success",
    [A5_ERR_FRAME_INVALID] = "frame value out of bounds",
    [A5_ERR_NOMEM] = "failed to allocate memory"
};

struct a5_cipher_state {
    size_t reg1;
    size_t reg2;
    size_t reg3;
};

/* returns each bit xor'ed with each other */
int odd_parity(size_t v) { 
    #if (ULONG_MAX > 0xFFFFFFFFu)
    v ^= v >> 32;
    #endif
    #if (ULONG_MAX > 0xFFFFu)
    v ^= v >> 16;
    #endif
    v ^= v >> 8;
    v ^= v >> 4;
    v ^= v >> 2;
    v ^= v >> 1;
    return (int) (v & 1);
}

/* shift register and calculate new bit */
void shift_register(size_t* reg, size_t mask) {
    *reg = (*reg << 1) | odd_parity(*reg & mask);
}

/* gets bit from integer at bit_pos */
size_t get_bit_from_int(int integer, size_t bit_pos) {
    return (integer & (1 << bit_pos)) >> bit_pos;
}

/* gets bit from char array at bit_pos */
size_t get_bit_from_array(const char* key, size_t bit_pos) {
    return get_bit_from_int(key[bit_pos / 8], bit_pos % 8);
}

/* writes bit to array at bit_pos */
void write_bit_to_pos(char* array, int bit, size_t bit_pos) {
    array[bit_pos / 8] |= bit << (bit_pos % 8); 
}

/* gets out bit for register */
int get_reg_out_bit(size_t reg, size_t size) {
    return (reg & (1 << (size - 1))) >> (size - 1);
}

/* gets majority bit for register */
int get_reg_maj_bit(size_t reg, size_t maj_pos) {
    return get_bit_from_int(reg, maj_pos);
}

struct a5_cipher_state* a5_new_cipher_state() {
    struct a5_cipher_state* state = malloc(sizeof(struct a5_cipher_state));
    a5_reset_state(state);
    return state;
}

int a5_write_key(struct a5_cipher_state* state, const char* key) {
    assert(state);
    assert(key);

    int i = A5_KEY_LEN;
    while (i--) {
        state->reg1 ^= get_bit_from_array(key, A5_KEY_LEN - 1 - i);
        state->reg2 ^= get_bit_from_array(key, A5_KEY_LEN - 1 - i);
        state->reg3 ^= get_bit_from_array(key, A5_KEY_LEN - 1 - i);
        shift_register(&state->reg1, A5_REG1_MASK);
        shift_register(&state->reg2, A5_REG2_MASK);
        shift_register(&state->reg3, A5_REG3_MASK);
    }

    return A5_ERR_OK;
}

int a5_write_frame(struct a5_cipher_state* state, size_t frame) {
    assert(state);

    int i = A5_FRAME_LEN;
    while (i--) {
        state->reg1 ^= get_bit_from_int(frame, A5_FRAME_LEN - 1 - i);
        state->reg2 ^= get_bit_from_int(frame, A5_FRAME_LEN - 1 - i);
        state->reg3 ^= get_bit_from_int(frame, A5_FRAME_LEN - 1 - i);
        shift_register(&state->reg1, A5_REG1_MASK);
        shift_register(&state->reg2, A5_REG2_MASK);
        shift_register(&state->reg3, A5_REG3_MASK);
    }

    return A5_ERR_OK;
}

int a5_iter(struct a5_cipher_state* state, int* bit) {
    assert(state);

    *bit = \
        get_reg_out_bit(state->reg1, A5_REG1_LEN)
        ^ get_reg_out_bit(state->reg2, A5_REG2_LEN)
        ^ get_reg_out_bit(state->reg3, A5_REG3_LEN);

    int reg1_maj = get_reg_maj_bit(state->reg1, A5_REG1_CLOCKING_BIT_POS);
    int reg2_maj = get_reg_maj_bit(state->reg2, A5_REG2_CLOCKING_BIT_POS);
    int reg3_maj = get_reg_maj_bit(state->reg3, A5_REG3_CLOCKING_BIT_POS);

    int maj = reg1_maj & reg2_maj | reg2_maj & reg3_maj | reg1_maj & reg3_maj;

    if (maj == reg1_maj) {
        shift_register(&state->reg1, A5_REG1_MASK);
    }
    if (maj == reg2_maj) {
        shift_register(&state->reg2, A5_REG2_MASK);
    }
    if (maj == reg3_maj) {
        shift_register(&state->reg3, A5_REG3_MASK);
    }

    return A5_ERR_OK;
}

void a5_reset_state(struct a5_cipher_state* state) {
    assert(state);

    memset(state, 0, sizeof(struct a5_cipher_state));
}

void a5_free_cipher_state(struct a5_cipher_state* state) {
    free(state);
}

int a5_cipher(
    struct a5_cipher_state* state,
    const char* key,
    size_t frame,
    const char* opentext,
    char* ciphertext
) {
    assert(opentext);
    assert(ciphertext);

    int error, bit, i;
    error = A5_ERR_OK;
    a5_reset_state(state);

    if ((error = a5_write_key(state, key))) {
        return error;
    }

    if ((error = a5_write_frame(state, frame))) {
        return error;
    }

    i = A5_DRY_ITERS_COUNT + A5_CHUNK_LEN;
    while (i--) {
        if ((error = a5_iter(state, &bit))) {
            return error;
        }

        if (i < A5_CHUNK_LEN) {
            int cipher_bit = get_bit_from_array(opentext, A5_CHUNK_LEN - 1 - i);
            write_bit_to_pos(ciphertext, cipher_bit ^ bit, A5_CHUNK_LEN - 1 - i);
        }
    }

    return error;
}

int a5_decipher(
    struct a5_cipher_state* state, 
    const char* key, 
    size_t frame, 
    const char* ciphertext, 
    char* opentext
) {
    return a5_cipher(state, key, frame, ciphertext, opentext);
}

const char* a5_error_string(enum a5_error error) {
    return errors[error];
}