#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "solver.h"
#include "test-suite-gen.h"

#define A5_DICTIONARY_LEN 10

static const char* const dictionary[A5_DICTIONARY_LEN] = {
    "hello",
    "world",
    "lol",
    "kek",
    "ilya",
    "ggrisha",
    "grisha",
    "g",
    "gg",
    "loves"
};

/* rounds bit len to nearest byte count to hold it */
size_t round_to_bytes(size_t bits) {
    return ceil((float) bits / 8);
}

int a5_gen_keys(char*** keys, size_t count, unsigned int seed) {
    srand(seed);

    char** gen = calloc(count, sizeof(char*));
    if (!gen) {
        return A5_ERR_NOMEM;
    }

    int i = count;
    size_t word_index;
    size_t word_len;
    while (i--) {
        word_index = rand() % A5_DICTIONARY_LEN;
        word_len = strlen(dictionary[word_index]);

        gen[i] = calloc(round_to_bytes(A5_KEY_LEN), sizeof(char));
        memcpy(gen[i], dictionary[word_index], word_len);
    }

    *keys = gen;

    return A5_ERR_OK;
}

int a5_gen_opentext(char*** opentexts, size_t count, unsigned int seed) {
    srand(seed);

    char** gen = calloc(count, sizeof(char*));
    if (!gen) {
        return A5_ERR_NOMEM;
    }

    int j, i = count;
    size_t word_index;
    size_t word_len;
    size_t sentence_len = round_to_bytes(A5_CHUNK_LEN);

    while (i--) {
        word_index = rand() % A5_DICTIONARY_LEN;
        word_len = strlen(dictionary[word_index]);

        gen[i] = calloc(sentence_len, sizeof(char));

        j = 0;
        while (j + word_len + 1 < sentence_len) {
            memcpy(gen[i] + j, dictionary[word_index], word_len);
            gen[i][j + word_len] = ' ';
            j += word_len + 1;

            word_index = rand() % A5_DICTIONARY_LEN;
            word_len = strlen(dictionary[word_index]);
        }

        gen[i][sentence_len - 1] = '\0';
    }

    *opentexts = gen;

    return A5_ERR_OK;
}

void a5_free_keys(char** keys, size_t count) {
    for (int i = 0; i < count; ++i) {
        free(keys[i]);
    }
    free(keys);
}

void a5_free_opentext(char** opentexts, size_t count) {
    for (int i = 0; i < count; ++i) {
        free(opentexts[i]);
    }
    free(opentexts);
}