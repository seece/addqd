#include <mmintrin.h>
#include <xmmintrin.h>

// these are from
// http://stackoverflow.com/a/6566033

float minss ( float a, float b )
{
    // Branchless SSE min.
    _mm_store_ss( &a, _mm_min_ss(_mm_set_ss(a),_mm_set_ss(b)) );
    return a;
}

float maxss ( float a, float b )
{
    // Branchless SSE max.
    _mm_store_ss( &a, _mm_max_ss(_mm_set_ss(a),_mm_set_ss(b)) );
    return a;
}

float clampss ( float val, float minval, float maxval )
{
    // Branchless SSE clamp.
    // return minss( maxss(val,minval), maxval );

    _mm_store_ss( &val, _mm_min_ss( _mm_max_ss(_mm_set_ss(val),_mm_set_ss(minval)), _mm_set_ss(maxval) ) );
    return val;
}