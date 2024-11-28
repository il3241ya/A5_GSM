#include <cmath>
#include <cstring>

#include <gtest/gtest.h>

#include "src/solver.h"

namespace {

    class CipherTest : public ::testing::Test {
    public:

        void SetUp() override {
            assert((state = a5_new_cipher_state()));
        }

        void TearDown() override {
            a5_free_cipher_state(state);
        }

        const std::size_t keyByteLen = std::ceil(static_cast<float>(A5_KEY_LEN) / 8);
        const std::size_t opentextByteLen = std::ceil(static_cast<float>(A5_FRAME_LEN) / 8);
        a5_cipher_state* state;

    };

}

TEST_F(CipherTest, TestSomething) {
    // empty
}