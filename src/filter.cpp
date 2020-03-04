/**
 * @file screen_ui.cpp
 */
#include <math.h>

#include "common.h"
#include "filter.h"

/**
 * @brief constructor
 */
Filter::Filter( float fs )
{
    fs_ = fs;

    a0_ = 1.0f; // 0以外にしておかないと除算でエラーになる
    a1_ = 0.0f;
    a2_ = 0.0f;
    b0_ = 1.0f;
    b1_ = 0.0f;
    b2_ = 0.0f;

    in1_ = in2_ = out1_ = out2_ = 0.0f;
}


/**
 * @brief constructor
 */
float Filter::Process( float in )
{
    // 入力信号にフィルタを適用し、出力信号変数に保存。
    float out = b0_ / a0_ * in + b1_ / a0_ * in1_ + b2_ / a0_ * in2_
        - a1_ / a0_ * out1_ - a2_ / a0_ * out2_;

    in2_ = in1_;
    in1_ = in;

    out2_ = out1_;
    out1_ = out;

    return out;
}

void Filter::LowPass(float freq, float q)
{
    float omega = 2.0f * 3.14159265f *  freq / fs_;
    float alpha = sin(omega) / (2.0f * q);

    a0_ = 1.0f + alpha;
    a1_ = -2.0f * cos(omega);
    a2_ = 1.0f - alpha;
    b0_ = (1.0f - cos(omega)) / 2.0f;
    b1_ = 1.0f - cos(omega);
    b2_ = (1.0f - cos(omega)) / 2.0f;
}

void Filter::HighPass(float freq, float q)
{
    float omega = 2.0f * 3.14159265f *  freq / fs_;
    float alpha = sin(omega) / (2.0f * q);

    a0_ = 1.0f + alpha;
    a1_ = -2.0f * cos(omega);
    a2_ = 1.0f - alpha;
    b0_ = (1.0f + cos(omega)) / 2.0f;
    b1_ = -(1.0f + cos(omega));
    b2_ = (1.0f + cos(omega)) / 2.0f;
}

void Filter::BandPass(float freq, float bw)
{
    float omega = 2.0f * 3.14159265f *  freq / fs_;
    float alpha = sin(omega) * sinh(log(2.0f) / 2.0 * bw * omega / sin(omega));

    a0_ = 1.0f + alpha;
    a1_ = -2.0f * cos(omega);
    a2_ = 1.0f - alpha;
    b0_ = alpha;
    b1_ = 0.0f;
    b2_ = -alpha;
}

void Filter::Notch(float freq, float bw)
{
    float omega = 2.0f * 3.14159265f *  freq / fs_;
    float alpha = sin(omega) * sinh(log(2.0f) / 2.0 * bw * omega / sin(omega));

    a0_ = 1.0f + alpha;
    a1_ = -2.0f * cos(omega);
    a2_ = 1.0f - alpha;
    b0_ = 1.0f;
    b1_ = -2.0f * cos(omega);
    b2_ = 1.0f;
}

void Filter::LowShelf(float freq, float q, float gain)
{
    float omega = 2.0f * 3.14159265f *  freq / fs_;
    float alpha = sin(omega) / (2.0f * q);
    float A = pow(10.0f, (gain / 40.0f));
    float beta = sqrt(A) / q;

    a0_ = (A + 1.0f) + (A - 1.0f) * cos(omega) + beta * sin(omega);
    a1_ = -2.0f * ((A - 1.0f) + (A + 1.0f) * cos(omega));
    a2_ = (A + 1.0f) + (A - 1.0f) * cos(omega) - beta * sin(omega);
    b0_ = A * ((A + 1.0f) - (A - 1.0f) * cos(omega) + beta * sin(omega));
    b1_ = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cos(omega));
    b2_ = A * ((A + 1.0f) - (A - 1.0f) * cos(omega) - beta * sin(omega));
}

void Filter::HighShelf(float freq, float q, float gain)
{
    float omega = 2.0f * 3.14159265f *  freq / fs_;
    float alpha = sin(omega) / (2.0f * q);
    float A = pow(10.0f, (gain / 40.0f));
    float beta = sqrt(A) / q;

    a0_ = (A + 1.0f) - (A - 1.0f) * cos(omega) + beta * sin(omega);
    a1_ = 2.0f * ((A - 1.0f) - (A + 1.0f) * cos(omega));
    a2_ = (A + 1.0f) - (A - 1.0f) * cos(omega) - beta * sin(omega);
    b0_ = A * ((A + 1.0f) + (A - 1.0f) * cos(omega) + beta * sin(omega));
    b1_ = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cos(omega));
    b2_ = A * ((A + 1.0f) + (A - 1.0f) * cos(omega) - beta * sin(omega));
}


void Filter::Peaking(float freq, float bw, float gain)
{
    float omega = 2.0f * 3.14159265f *  freq / fs_;
    float alpha = sin(omega) * sinh(log(2.0f) / 2.0 * bw * omega / sin(omega));
    float A = pow(10.0f, (gain / 40.0f));

    a0_ = 1.0f + alpha / A;
    a1_ = -2.0f * cos(omega);
    a2_ = 1.0f - alpha / A;
    b0_ = 1.0f + alpha * A;
    b1_ = -2.0f * cos(omega);
    b2_ = 1.0f - alpha * A;
}

void Filter::AllPass(float freq, float q)
{
    float omega = 2.0f * 3.14159265f *  freq / fs_;
    float alpha = sin(omega) / (2.0f * q);

    a0_ = 1.0f + alpha;
    a1_ = -2.0f * cos(omega);
    a2_ = 1.0f - alpha;
    b0_ = 1.0f - alpha;
    b1_ = -2.0f * cos(omega);
    b2_ = 1.0f + alpha;
}