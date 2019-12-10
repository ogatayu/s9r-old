/**
 * @file common.h
 */
#pragma once

#include <chrono>

#define MIN(a,b)    ((a)<(b)?(a):(b))
#define MAX(a,b)    ((a)>(b)?(a):(b))
#define PI          (3.141592653589793238462643383279f)

static std::chrono::system_clock::time_point  commonMesureTimeStart;

/**
 * @brief CMN_MEASURE_START
 */
inline void CMN_MEASURE_START( void )
{
    commonMesureTimeStart = std::chrono::system_clock::now();
}

/**
 * @brief CMN_MEASURE_END
 */
inline void CMN_MEASURE_END( const char* msg )
{
    fprintf( stderr, "%s : %0.0f [ms]\n",
             msg, (double)std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now()-commonMesureTimeStart ).count());
}
