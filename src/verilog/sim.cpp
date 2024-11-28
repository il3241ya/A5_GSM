#include <memory>
#include <ostream>
#include <cstdint>
#include <iostream>

#include <verilated.h>

#include "src/solver.h"
#include "src/test-suite-gen.h"

#include "vcipher.h"

#define CLOCK(context, model)       \
    model->clock = !model->clock;   \
    context->timeInc(1);            \
    model->eval()

std::ostream& print_byte(std::ostream& os, char byte) {
    std::uint8_t ubyte = reinterpret_cast<std::uint8_t&>(byte);
    if (ubyte <= 0xF) {
        os << "0x0" << static_cast<int>(ubyte);
    } else {
        os << "0x" << static_cast<int>(ubyte);
    }
    return os;
}

std::size_t roundToBytes(std::size_t bits) {
    return std::ceil(static_cast<float>(bits) / 8);
}

template<int Width>
void fillSeq(VlWide<Width>& seq, const char* key, std::size_t frame) {
    for (std::size_t i = 0; i < A5_KEY_LEN + A5_FRAME_LEN; ++i) {
        if (i < A5_KEY_LEN) {
            seq[i / (sizeof(std::size_t) * 8)] |= \
                key[i / sizeof(char)] & (1 << (i % sizeof(char)))
                >> (i % sizeof(char)) << (i % (sizeof(std::size_t) * 8));
        } else {
            seq[i / (sizeof(std::size_t) * 8)] |= \
                frame & (1 << (i - A5_KEY_LEN));
        }
    }
}

int main(int argc, char** argv) {
    auto context = std::make_unique<VerilatedContext>();

    context->traceEverOn(true);
    context->commandArgs(argc, argv);

    auto model = std::make_unique<vcipher>(context.get(), "cipher");

    model->clock = 0;
    model->control = 0;
    model->in = 0;
    model->out = 0;
    model->reset = 0;

    model->seq[0] = 0;
    model->seq[1] = 0;
    model->seq[2] = 0;

    const unsigned int seed = 42;
    const std::size_t seqWidth = 3;
    const std::size_t testsCount = 5;

    int i, j;
    char** keys = nullptr;
    char** opentexts = nullptr;
    a5_cipher_state* state = nullptr;

    if (int error = a5_gen_keys(&keys, testsCount, seed); error != A5_ERR_OK) {
        std::cout << "- error during keys generation: " << a5_error_string((a5_error) error) << std::endl;
        goto early_0;
    }

    std::cout << "- keys generated: \n";
    for (i = 0; i < testsCount; ++i) {
        std::cout << "-- " << keys[i] << '\n';
    }

    if (int error = a5_gen_opentext(&opentexts, testsCount, seed); error != A5_ERR_OK) {
        std::cout << "- error during opentexts generation: " << a5_error_string((a5_error) error) << std::endl;
        goto early_1;
    }

    std::cout << "- opentexts generated: \n";
    for (i = 0; i < testsCount; ++i) {
        std::cout << "-- " << opentexts[i] << '\n';
    }

    if (state = a5_new_cipher_state(); !state) {
        std::cout << "- failed to allocate memory for cipher state" << std::endl;
        goto early_2;
    }

    i = testsCount;
    while (i--) {
        std::size_t keyLen = roundToBytes(A5_KEY_LEN);
        std::size_t chunkLen = roundToBytes(A5_CHUNK_LEN);

        char* currentKey = keys[i];
        char* currentOpentext = opentexts[i];

        // set opentext to 0 to see generated masks
        // std::memset(currentOpentext, 0, chunkLen);

        auto seqInput = std::make_unique<std::size_t>(seqWidth);
        auto result = std::make_unique<char[]>(chunkLen);

        // assign key & frame
        const std::size_t frame = 1;

        // run library code first
        std::cout << "#Test " << testsCount - i << "\n";

        if (int error = a5_cipher(
            state, currentKey, frame, currentOpentext, result.get()
        ); error != A5_ERR_OK) {
            std::cout << "Error: cipher failed: " << a5_error_string((a5_error) error) << std::endl;
            goto early_3;
        }

        std::cout << "- checker generated output: \n";
        std::cout << "-- " << std::hex;
        for (j = 0; j < chunkLen; ++j) {
            print_byte(std::cout, result[j]) << ' ';
        }
        std::cout << std::dec << "\n";

        // now we run the actual model
        // it runs: 64 + 22 (key + frame len cycles)
        // then: 100 dry runs
        // and finally 114 runs for chunk
        j = A5_KEY_LEN + A5_FRAME_LEN + A5_DRY_ITERS_COUNT + A5_CHUNK_LEN;
        // we fill 86 bits for seq first
        fillSeq<seqWidth>(model->seq, currentKey, frame);
        std::memset(result.get(), 0, chunkLen);
        while (j--) {
            if (j >= A5_DRY_ITERS_COUNT + A5_CHUNK_LEN) {
                model->reset = 1;
                model->control = 0;
                CLOCK(context, model);
                CLOCK(context, model);
            } else if (j >= A5_CHUNK_LEN) {
                model->reset = 0;
                model->control = 0;
                CLOCK(context, model);
                CLOCK(context, model);
            } else {
                model->reset = 0;
                model->control = 1;
                model->in = currentOpentext[(A5_CHUNK_LEN - i) / 8] >> ((A5_CHUNK_LEN - i) % 8);
                CLOCK(context, model);
                result[(A5_CHUNK_LEN - j) / 8] |= model->out << ((A5_CHUNK_LEN - j) % 8);
                CLOCK(context, model);
            }
        }

        std::cout << "- simulation complete, resulting sequence: \n";
        std::cout << "-- " << std::hex;
        for (j = 0; j < chunkLen; ++j) {
            print_byte(std::cout, result[j]) << ' ';
        }
        std::cout << std::dec << "\n";
    }

    model->final();
    context->statsPrintSummary();

early_3:
    a5_free_cipher_state(state);
early_2:
    a5_free_opentext(opentexts, testsCount);
early_1:
    a5_free_keys(keys, testsCount);
early_0:
    return 0;
}