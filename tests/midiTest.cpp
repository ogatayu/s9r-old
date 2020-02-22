#include <gtest/gtest.h>

#include <math.h>
#include "midi.h"

namespace{
    class MidiTest : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
            MidiCtrl::Create();
        }

        virtual void TearDown()
        {
            MidiCtrl::Destroy();
        }
    };

    TEST_F(MidiTest, GetInstance){
        MidiCtrl* midictrl = MidiCtrl::GetInstance();
        EXPECT_NE( nullptr, midictrl );
    }
}