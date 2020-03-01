#include <gtest/gtest.h>

#include <math.h>

#include "midi.h"
#include "audio.h"
#include "synth.h"
#include "draw.h"

namespace {
    class SynthTest : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
            MidiCtrl::Create();
            AudioCtrl::Create();
            Synth::Create( 440.0 );
            Draw::Create();
        }

        virtual void TearDown()
        {
            Draw::Destroy();
            Synth::Destroy();
            AudioCtrl::Destroy();
            MidiCtrl::Destroy();
        }
    };

    TEST_F(SynthTest, Create)
    {
        Synth* synth = Synth::Create( 440.0 );
        EXPECT_NE( nullptr, synth );
    }


    TEST_F(SynthTest, GetInstance)
    {
        Synth* synth = Synth::GetInstance();
        EXPECT_NE( nullptr, synth );
    }

    TEST_F(SynthTest, SignalCallback)
    {
        Synth* synth = Synth::GetInstance();
        EXPECT_EQ( 0.0, synth->SignalCallback() );
    }
}

namespace {
    class VoiceCtrlTest : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
            MidiCtrl::Create();
            AudioCtrl::Create();
            Synth::Create( 440.0 );
        }

        virtual void TearDown()
        {
            Synth::Destroy();
            AudioCtrl::Destroy();
            MidiCtrl::Destroy();
        }
    };

    TEST_F(VoiceCtrlTest, Trigger)
    {
        VoiceCtrl voicectrl;

        voicectrl.Trigger();
    }
}