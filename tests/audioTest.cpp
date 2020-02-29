#include <gtest/gtest.h>

#include "audio.h"

namespace {
    class AudioTest : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
            audio_ = AudioCtrl::Create();
        }

        virtual void TearDown()
        {
            AudioCtrl::Destroy();
            audio_ = nullptr;
        }

    public:
        AudioCtrl* audio_;
    };

    TEST_F(AudioTest, Create)
    {
        audio_ = AudioCtrl::Create();
        EXPECT_NE( nullptr, audio_ );
    }

    TEST_F(AudioTest, GetInstance)
    {
        audio_ = AudioCtrl::GetInstance();
        EXPECT_NE( nullptr, audio_ );
    }

    TEST_F(AudioTest, SampleRateGet)
    {
        audio_ = AudioCtrl::GetInstance();
        EXPECT_LT( 0, audio_->SampleRateGet() );
    }
}
