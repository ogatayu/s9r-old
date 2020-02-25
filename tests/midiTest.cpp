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

    TEST_F(MidiTest, GetInstance)
    {
        MidiCtrl* midictrl = MidiCtrl::GetInstance();
        EXPECT_NE( nullptr, midictrl );
    }

    TEST_F(MidiTest, IsStatusChanged)
    {
        MidiCtrl* midictrl = MidiCtrl::GetInstance();
        EXPECT_EQ( false, midictrl->IsStatusChanged() );
    }

    TEST_F(MidiTest, ResetStatusChange)
    {
        MidiCtrl* midictrl = MidiCtrl::GetInstance();

        // status must be false after this function has called.
        midictrl->ResetStatusChange();
        EXPECT_EQ( false, midictrl->IsStatusChanged() );
    }

    TEST_F(MidiTest, GetVelocity)
    {
        MidiCtrl* midictrl = MidiCtrl::GetInstance();
        EXPECT_EQ( 0, midictrl->GetVelocity(0) );
        EXPECT_EQ( 0, midictrl->GetVelocity(127) );
    }

    TEST_F(MidiTest, GetOnKeyNum)
    {
        MidiCtrl* midictrl = MidiCtrl::GetInstance();
        EXPECT_EQ( 0, midictrl->GetOnKeyNum() );
    }

    TEST_F(MidiTest, GetOnKeyNN)
    {
        MidiCtrl* midictrl = MidiCtrl::GetInstance();
        EXPECT_EQ( -1, midictrl->GetOnKeyNN(0) );
        EXPECT_EQ( -1, midictrl->GetOnKeyNN(1) );
    }

    TEST_F(MidiTest, GetNewOnKeyNN)
    {
        MidiCtrl* midictrl = MidiCtrl::GetInstance();
        EXPECT_EQ( -1, midictrl->GetNewOnKeyNN(0) );
        EXPECT_EQ( -1, midictrl->GetNewOnKeyNN(1) );
    }
}