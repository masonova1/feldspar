#include "feldspar.h"

int fflog2_s16(const int16_t x)
{
    if(x & 0x7F00)
    {
        if(x & 0x7000)
        {
            if(x & 0x4000) return 15;
            else return (x & 0x2000) ? 14 : 13;
        }
        else
        {
            if(x & 0x0C00) return (x & 0x0800) ? 12 : 11;
            else return (x & 0x0200) ? 10 : 9;
        }
    }
    else
    {
        if(x & 0x00F0)
        {
            if(x & 0x00C0) return (x & 0x0080) ? 8 : 7;
            else return (x & 0x0020) ? 6 : 5;
        }
        else
        {
            if(x & 0x000C) return (x & 0x0008) ? 4 : 3;
            else return (x & 0x0002) ? 2 : (x ? 1 : 0);
        }
    }
}

int fflog2_u16(const uint16_t x)
{
    if(x & 0xFF00)
    {
        if(x & 0xF000)
        {
            if(x & 0xC000) return (x & 0x8000) ? 16 : 15;
            else return (x & 0x2000) ? 14 : 13;
        }
        else
        {
            if(x & 0x0C00) return (x & 0x0800) ? 12 : 11;
            else return (x & 0x0200) ? 10 : 9;
        }
    }
    else
    {
        if(x & 0x00F0)
        {
            if(x & 0x00C0) return (x & 0x0080) ? 8 : 7;
            else return (x & 0x0020) ? 6 : 5;
        }
        else
        {
            if(x & 0x000C) return (x & 0x0008) ? 4 : 3;
            else return (x & 0x0002) ? 2 : (x ? 1 : 0);
        }
    }
}

int fflog2_s32(const int32_t x)
{
    if(x & 0x7FFF0000)
    {
        if(x & 0x7F000000)
        {
            if(x & 0x70000000)
            {
                if(x & 0x40000000) return 31;
                else return (x & 0x20000000) ? 30 : 29;
            }
            else
            {
                if(x & 0x0C000000) return (x & 0x08000000) ? 28 : 27;
                else return (x & 0x02000000) ? 26 : 25;
            }
        }
        else
        {
            if(x & 0x00F00000)
            {
                if(x & 0x00C00000) return (x & 0x00800000) ? 24 : 23;
                else return (x & 0x00200000) ? 22 : 21;
            }
            else
            {
                if(x & 0x000C0000) return (x & 0x00080000) ? 20 : 19;
                else return (x & 0x00020000) ? 18 : 17;
            }
        }
    }
    else
    {
        if(x & 0xFF00)
        {
            if(x & 0xF000)
            {
                if(x & 0xC000) return (x & 0x8000) ? 16 : 15;
                else return (x & 0x2000) ? 14 : 13;
            }
            else
            {
                if(x & 0x0C00) return (x & 0x0800) ? 12 : 11;
                else return (x & 0x0200) ? 10 : 9;
            }
        }
        else
        {
            if(x & 0x00F0)
            {
                if(x & 0x00C0) return (x & 0x0080) ? 8 : 7;
                else return (x & 0x0020) ? 6 : 5;
            }
            else
            {
                if(x & 0x000C) return (x & 0x0008) ? 4 : 3;
                else return (x & 0x0002) ? 2 : (x ? 1 : 0);
            }
        }
    }
}

fs::fxp_t fs::fxp_sqrt(const fxp_t _x)
{
    if(_x == 0) return 0;
    #ifdef FXP_32_BITS
    int flog2o2 = (fflog2_s32(_x) - 16) >> 1; // Pretty close to floor(log2(x))*0.5
    fxp_t g = _x >> flog2o2;
    int64_t xs = int64_t(_x) << 16;
    g = (g + (xs / g)) >> 1;
    g = (g + (xs / g)) >> 1;
    g = (g + (xs / g)) >> 1;
    #else
    int flog2o2 = (fflog2_s16(_x) - 6) >> 1;
    fxp_t g = _x >> flog2o2;
    int32_t xs = int32_t(_x) << 6;
    g = (g + (xs / g)) >> 1;
    g = (g + (xs / g)) >> 1;
    #endif
    return g;
}

fs::fxp_t fs::fxp_hypot(const fxp_t _a, const fxp_t _b)
{
    int64_t a2 = _a;
    a2 *= a2;
    int64_t b2 = _b;
    b2 *= b2;
    int64_t s = (a2 + b2) >> 16;
    int scale = 0;
    while(s > 0x7FFFFFFF)
    {
        // sqrt(x) = 2*sqrt(x/4)
        s >>= 2;
        scale++;
        // In theory, this could just as easily exploit sqrt(x) = 4*sqrt(x/16), or any
        // other power-of-two-amenable form of the identity sqrt(x) = sqrt(a)*sqrt(x/a),
        // but this maintains as much accuracy as reasonably possible.

        // This could exploit a modified fflog2 function to just compute how many bits over
        // the limit we are and shift correctly without any branching or loops, but I would
        // have to test it to see if it was actually faster than this.
    }
    return fxp_sqrt((fxp_t)s) << scale;
}

//Multiplies as if _c is s7.24 coefficient for high accuracy, RETURNS COEFFICIENT TYPE s7.24
constexpr fs::fxp_t coeffMultiply(const fs::fxp_t _x, const int32_t _c)
{
    return (int64_t(_x) * _c) >> 16;
}

constexpr int32_t f2coeff(const float _x)
{
    return _x * 16777216.0 + 0.5;
}

//Very accurate piecewise polynomial for sin(x/16) inside 0 <= x <= 8pi.
static const int32_t coeffs[3][5] =
{
    {f2coeff(1.55993e-7f), f2coeff(-0.0000394463f), f2coeff(-0.000479209f), f2coeff(0.0605896f), f2coeff(0.245255f)},
    {f2coeff(4.37026e-7f), f2coeff(-0.0000295531f), f2coeff(-0.00134254f), f2coeff(0.0453935f), f2coeff(0.687382f)},
    {f2coeff(6.19241e-7f), f2coeff(-9.22133e-6f), f2coeff(-0.00190231), f2coeff(0.014164), f2coeff(0.973982f)},
};


//returns sin(x / 16) very accurately in the range 0 <= x <= 8pi
fs::fxp_t sinHalfbridge(fs::fxp_t _x)
{
    if(!_x) return 0;
    if(_x < 520948)
    {
        //Use first polynomial
        _x -= 259927;
        int32_t x2 = fs::fxp_mul(_x, _x);
        int32_t x3 = fs::fxp_mul(x2, _x);
        int32_t x4 = fs::fxp_mul(x3, _x);
        return ((coeffMultiply(x4, coeffs[0][0]) + coeffMultiply(x3, coeffs[0][1]) + coeffMultiply(x2, coeffs[0][2]) + coeffMultiply(_x, coeffs[0][3]) + coeffs[0][4]) + 0x80) >> 8;
    }
    else if(_x < 1073032)
    {
        //Use second polynomial
        _x -= 794693;
        int32_t x2 = fs::fxp_mul(_x, _x);
        int32_t x3 = fs::fxp_mul(x2, _x);
        int32_t x4 = fs::fxp_mul(x3, _x);
        return ((coeffMultiply(x4, coeffs[1][0]) + coeffMultiply(x3, coeffs[1][1]) + coeffMultiply(x2, coeffs[1][2]) + coeffMultiply(_x, coeffs[1][3]) + coeffs[1][4]) + 0x80) >> 8;
    }
    else
    {
        // Use third polynomial
        _x -= 1407385;
        int32_t x2 = fs::fxp_mul(_x, _x);
        int32_t x3 = fs::fxp_mul(x2, _x);
        int32_t x4 = fs::fxp_mul(x3, _x);
        return ((coeffMultiply(x4, coeffs[2][0]) + coeffMultiply(x3, coeffs[2][1]) + coeffMultiply(x2, coeffs[2][2]) + coeffMultiply(_x, coeffs[2][3]) + coeffs[2][4]) + 0x80) >> 8;
    }
}

// Returns sin(x/16)

fs::fxp_t fs::fxp_sin_internal(fxp_t _x)
{
    if(_x < 0) return -fs::fxp_sin_internal(-_x);
    _x %= 6588397;
    if(_x <= 1647099) return sinHalfbridge(_x);
    if(_x <= 3294199) return sinHalfbridge(1647099-(_x-1647099));
    if(_x <= 4941298) return -sinHalfbridge(_x - 3294199);
    else return -sinHalfbridge(1647099-(_x-4941298));
}

// Returns cos(x/16)

fs::fxp_t fs::fxp_cos_internal(fxp_t _x)
{
    if(_x < 0) _x = -_x;
    _x %= 6588397;
    if(_x <= 1647099) return sinHalfbridge(1647099-_x);
    if(_x <= 3294199) return -sinHalfbridge(_x-1647099);
    if(_x <= 4941298) return -sinHalfbridge(1647099-(_x-3294199));
    else return sinHalfbridge(_x-4941298);
}

fs::fxp_t fs::fxp_sindeg(const fxp_t _x){return fs::fxp_sin_internal(((int64_t(_x) * 4685082) >> 24));}
fs::fxp_t fs::fxp_cosdeg(const fxp_t _x){return fs::fxp_cos_internal(((int64_t(_x) * 4685082) >> 24));}

fs::fxp_t fs::fxp_sin(const fxp_t _x){return fs::fxp_sin_internal(_x << 4);}
fs::fxp_t fs::fxp_cos(const fxp_t _x){return fs::fxp_cos_internal(_x << 4);}
