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
        EXPECT_EQ  ( sin(0.0     * PI), wf->GetSine(0.0     * PI) );         // 0 * pi
        EXPECT_NEAR( sin(1.0/2.0 * PI), wf->GetSine(1.0/2.0 * PI), 0.00001); // 1/2 * pi
        EXPECT_NEAR( sin(1.0     * PI), wf->GetSine(1.0     * PI), 0.00001); // pi
        EXPECT_NEAR( sin(3.0/2.0 * PI), wf->GetSine(3.0/2.0 * PI), 0.00001); // 3/2 * pi
    }

    TEST_F(WaveformTest, GetTriangle){
        Waveform* wf = Waveform::GetInstance();
        int p = 0;
        uint32_t w = wf->CalcWFromFreq( 440.0 );
        for( int ix=0; ix<1000; ix++) {
            float val = wf->GetTriangle( 440.0, p );
            printf("%f\n", ix, val);
            p += w;
        }
    }
}
