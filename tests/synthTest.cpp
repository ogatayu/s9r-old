#include <gtest/gtest.h>

#include <math.h>
#include "synth.h"

namespace{
    class SynthTest : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
            Synth::Create();
        }

        virtual void TearDown()
        {
            Synth::Destroy();
        }
    };

    TEST_F(SynthTest, GetInstance){
        Synth* synth = Synth::GetInstance();
        EXPECT_NE( nullptr, synth );
    }
}