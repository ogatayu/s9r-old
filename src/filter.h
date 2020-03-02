/**
 * @file filter.h
 */
#pragma once

/**
 * @class Filter
 */
class Filter {
private:
    // sample rate
    float fs_;

    float a0_, a1_, a2_, b0_, b1_, b2_;
    float in1_, in2_, out1_, out2_;

public:
    Filter( float fs );
    ~Filter(){}

    // 入力信号にフィルタを適用する関数
    float Process(float in);

    void LowPass  (float freq, float q );
    void HighPass (float freq, float q );
    void BandPass (float freq, float bw);
    void Notch    (float freq, float bw);
    void LowShelf (float freq, float q , float gain);
    void HighShelf(float freq, float q , float gain);
    void Peaking  (float freq, float bw, float gain);
    void AllPass  (float freq, float q );
};
