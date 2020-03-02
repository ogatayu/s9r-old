#include <gtest/gtest.h>

#include <math.h>
#include "waveform.h"

#define PI (3.141592653589793238462643383279f)

namespace{
    class WaveformTest : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
            Waveform::Create( 440.0, 44100.0 );
        }

        virtual void TearDown()
        {
            Waveform::Destroy();
        }
    };

    TEST_F(WaveformTest, GetInstance){
        Waveform* wf = Waveform::GetInstance();
        EXPECT_NE( nullptr, wf );
    }

    TEST_F(WaveformTest, GetSine){
        Waveform* wf = Waveform::GetInstance();
        // 0 * pi
        EXPECT_EQ(
            sin(0.0 * PI),
            wf->GetSine( (WT_SIZE<<16) * 0.0 )
            );

        // 1/2 * pi
        EXPECT_NEAR(
            sin(1.0/2.0 * PI),
            wf->GetSine( (WT_SIZE<<16) * (1.0/2.0) * 0.5 ),
            0.00001
            );

        // pi
        EXPECT_NEAR(
            sin(1.0 * PI),
            wf->GetSine( (WT_SIZE<<16) * 1.0 * 0.5 ),
            0.00001
            );

        // 3/2 * pi
        EXPECT_NEAR(
            sin(3.0/2.0 * PI),
            wf->GetSine( (WT_SIZE<<16) * (3.0/2.0) * 0.5 ),
            0.00001
            );
    }

    TEST_F(WaveformTest, GetTriangle){
        Waveform* wf = Waveform::GetInstance();
        int p = 0;
        uint32_t w = wf->CalcWFromFreq( 69.f );
        for( int ix=0; ix<256; ix++) {
            float val = wf->GetTriangle( 69.f, p );
            printf("%f,", val);
            p += w;
        }
        printf("\n");
    }

    TEST_F(WaveformTest, GetSaw){
        Waveform* wf = Waveform::GetInstance();
        int p = 0;
        uint32_t w = wf->CalcWFromFreq( 69.f );
        for( int ix=0; ix<256; ix++) {
            float val = wf->GetSaw( 69.f, p );
            printf("%f,", val);
            p += w;
        }
        printf("\n");
    }

    TEST_F(WaveformTest, GetSquare){
        Waveform* wf = Waveform::GetInstance();
        int p = 0;
        uint32_t w = wf->CalcWFromFreq( 69.f );
        for( int ix=0; ix<256; ix++) {
            float val = wf->GetSquare( 69.f, p );
            printf("%f,", val);
            p += w;
        }
        printf("\n");
    }

    TEST_F(WaveformTest, CalcWFromNoteNo){
        Waveform* wf = Waveform::GetInstance();

        // A4(440Hz)
        EXPECT_EQ( wf->CalcWFromNoteNo( 69, 0 ), wf->CalcWFromFreq( 440.0 ) );

        // A5(880Hz)
        EXPECT_EQ( wf->CalcWFromNoteNo( 81, 0 ), wf->CalcWFromFreq( 880.0 ) );
    }
}
