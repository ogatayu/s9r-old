#include <gtest/gtest.h>

#include "fifo.h"

namespace {
    class FifoTest : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }

    public:
    };

    TEST_F(FifoTest, Create)
    {
        FIFO* fifo = new FIFO( 32, sizeof(int) );
        EXPECT_NE( nullptr, fifo );
    }
}
