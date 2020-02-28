#include <gtest/gtest.h>

#include "audio.h"

namespace {
    class AudioTest : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
            AudioCtrl::Create();
        }

        virtual void TearDown()
        {
            AudioCtrl::Destroy();
        }
    };

    TEST(AudioTest, Create)
    {
        AudioCtrl* audio = AudioCtrl::Create();
        EXPECT_NE( nullptr, audio );
    }

    TEST_F(AudioTest, GetInstance)
    {
        AudioCtrl* audio = AudioCtrl::GetInstance();
        EXPECT_NE( nullptr, audio );
    }
}
