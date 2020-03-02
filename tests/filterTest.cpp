#include <gtest/gtest.h>

#include <math.h>
#include "filter.h"

namespace{
    class FilterTest : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };

    TEST_F(FilterTest, Process)
    {
        // てきとう
        Filter filter( 48000.f );
        filter.Process( 0.5f );
    }
}
