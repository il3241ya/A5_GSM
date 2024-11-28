#ifndef A5_TEST_SUITE_GEN_H
#define A5_TEST_SUITE_GEN_H

#include "solver.h"

A5_DECL_BEGIN

/* generates keys from static dictionary, returns 0 on success */
int a5_gen_keys(char*** keys, size_t count, unsigned int seed);

/* generates opentext from the same static dictionary, returns 0 on success */
int a5_gen_opentext(char*** opentexts, size_t count, unsigned int seed);

/* frees memory for generated keys */
void a5_free_keys(char** keys, size_t count);

/* frees memory for opentexts */
void a5_free_opentext(char** opentexts, size_t count);

A5_DECL_END

#endif