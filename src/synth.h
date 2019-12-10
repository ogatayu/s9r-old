/**
 * @file synth.h
 */
#pragma once

extern bool synthInit( void );
extern float synthGetWave( int phase );
extern unsigned int synthGetW(int nn, float det, float fs);
