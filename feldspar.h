#ifndef FELDSPAR_H_INCLUDED
#define FELDSPAR_H_INCLUDED
#include <cstdint>

#define FXP_32_BITS // Comment out for lower fixed-point precision s9.6 numbers

#ifdef FXP_32_BITS
#define FRACBITS 16
#define POWTWO 65536.0
typedef int64_t large_t;
#else
#define FRACBITS 6
#define POWTWO 64.0
typedef int32_t large_t;
#endif

#define COLOR_16
//#define COLOR_32 // replace with this if colors are 32-bit


namespace fs
{
    // Platform-agnostic fixed-point type
    #ifdef FXP_32_BITS
    typedef int32_t fxp_t; // s15.16
    #else
    typedef int16_t fxp_t; // s9.6
    #endif

    // Primitive maths
    constexpr fxp_t f2fx(const float _x)
    {
        return (_x > 0) ? fxp_t(_x * POWTWO + 0.5) : fxp_t(_x * POWTWO - 0.5);
    }
    constexpr float fx2f(const fxp_t _x)
    {
        return float(_x) / POWTWO;
    }
    constexpr fxp_t fxp_add(const fxp_t _a, const fxp_t _b) { return _a + _b; };
    constexpr fxp_t fxp_sub(const fxp_t _a, const fxp_t _b) { return _a - _b; };
    constexpr fxp_t fxp_mul(const fxp_t _a, const fxp_t _b)
    {
        return (large_t(_a) * _b) >> FRACBITS;
    }
    constexpr fxp_t fxp_div(const fxp_t _a, const fxp_t _b)
    {
        return (large_t(_a) << FRACBITS) / _b;
    }
    //Branchless max/min, abs, and sign functions
    constexpr fxp_t fxp_max(fxp_t _a, fxp_t _b)
    {
        #ifdef FXP_32_BITS
        return _a-((_a-_b)&(_a-_b)>>31);
        #else
        return _a-((_a-_b)&(_a-_b)>>15);
        #endif
    }
    constexpr fxp_t fxp_min(fxp_t _a, fxp_t _b)
    {
        #ifdef FXP_32_BITS
        return _b+((_a-_b)&(_a-_b)>>31);
        #else
        return _b+((_a-_b)&(_a-_b)>>15);
        #endif
    }
    constexpr fxp_t fxp_abs(const fxp_t _x)
    {
        #ifdef FXP_32_BITS
        return (_x ^ (_x >> 31)) - (_x >> 31);
        #else
        return (_x ^ (_x >> 15)) - (_x >> 15);
        #endif
    }
    constexpr fxp_t fxp_sgn(const fxp_t _x)
    {
        #ifdef FXP_32_BITS
        return (((uint32_t)-_x >> 31) - ((uint32_t)_x >> 31)) << 16;
        #else
        return (((uint32_t)-_x >> 15) - ((uint32_t)_x >> 15)) << 6;
        #endif
    }
    constexpr fxp_t fxp_floor(const fxp_t _x)
    {
        #ifdef FXP_32_BITS
        return _x & 0xFFFF0000;
        #else
        return _x & 0xFFC0;
        #endif
    }
    constexpr fxp_t fxp_ceil(const fxp_t _x)
    {
        #ifdef FXP_32_BITS
        return (_x + 0xFFFF) & 0xFFFF0000;
        #else
        return (_x + 0x3F) & 0xFFC0;
        #endif
    }
    constexpr fxp_t fxp_round(const fxp_t _x)
    {
        #ifdef FXP_32_BITS
        return (_x + 0x8000) & 0xFFFF0000;
        #else
        return (_x + 0x20) & 0xFFC0;
        #endif
    }

    fxp_t fxp_sqrt(const fxp_t);
    fxp_t fxp_hypot(const fxp_t, const fxp_t); // If points are far (~180 units or more) apart, this will avoid overflow errors, but is not perfectly accurate

    // sin(x/16) and cos(x/16) respectively
    fxp_t fxp_sin_internal(const fxp_t);
    fxp_t fxp_cos_internal(const fxp_t);
    // Adapted for degrees
    fxp_t fxp_sindeg(const fxp_t);
    fxp_t fxp_cosdeg(const fxp_t);
    // ...And normal radians
    fxp_t fxp_sin(const fxp_t);
    fxp_t fxp_cos(const fxp_t);

    // Platform-agnostic fixed-point 2d vector type, 4 or 8 bytes per depending on fixed type
    struct fxVec2
    {
        int32_t x, y;

        fxVec2(void);
        fxVec2(const fxp_t, const fxp_t);
        fxVec2(const float, const float);
        fxVec2(const fxVec2&);

        fxVec2 operator+(const fxVec2&);
        fxVec2 operator-(const fxVec2&);
        fxVec2 operator*(const fxVec2&);
        fxVec2 operator/(const fxVec2&);
        fxVec2 operator+(const fxp_t);
        fxVec2 operator-(const fxp_t);
        fxVec2 operator*(const fxp_t);
        fxVec2 operator/(const fxp_t);
        fxVec2 operator+=(const fxVec2&);
        fxVec2 operator-=(const fxVec2&);
        fxVec2 operator*=(const fxVec2&);
        fxVec2 operator/=(const fxVec2&);
        fxVec2 operator+=(const fxp_t);
        fxVec2 operator-=(const fxp_t);
        fxVec2 operator*=(const fxp_t);
        fxVec2 operator/=(const fxp_t);

        bool operator==(const fxVec2&);
        bool operator!=(const fxVec2&);
        //THESE COMPARE ABSOLUTE LENGTH!
        bool operator>(const fxVec2&);
        bool operator>=(const fxVec2&);
        bool operator<(const fxVec2&);
        bool operator<=(const fxVec2&);

        fxVec2 normal(); // Unit vector pointing in same direction
        fxVec2 ltan(); // Left-hand perpendicular (tangent) vector
        fxVec2 rtan(); // Right-hand perpendicular (tangent) vector
        fxVec2 rotation(const fxp_t); // USE SPARINGLY! Takes degrees
        fxVec2 rotation(const fxp_t, const fxp_t); // Need to compute sin(x) and cos(x) beforehand

        fxp_t len2();
        fxp_t len(); // ONLY USE ME IF YOU NEED ME
        fxp_t dist2(const fxVec2&);
        fxp_t dist(const fxVec2&); // ME TOO
        fxp_t dot(const fxVec2&);   // x1x2+y1y2
        fxp_t cross(const fxVec2&); // x1y2-y1x2
    };

    // Collider objects
    struct BCCOllider; // Bounding circle collider
    struct BBCollider; // Bounding box collider
    struct CGCollider; // Convex geometry collider

    struct BCCollider
    {
        fxVec2 pos;
        fxp_t r;
        BCCollider(void);
        BCCollider(const fxVec2&, const fxp_t);
        BCCollider(const BCCollider&);
        
        fxVec2 center(); // UNIMPLEMENTED

        // Potentially faster than actually computing MTV
        bool testPoint(const fxVec2&);
        bool isCollidingWith(BCCollider&);
        bool isCollidingWith(BBCollider&);
        bool isCollidingWith(CGCollider&);

        // Compute minimum translation vector
        fxVec2 collisionMTV(BCCollider&, fxVec2&);
        fxVec2 collisionMTV(BCCollider&);
        fxVec2 collisionMTV(BBCollider&, fxVec2&);
        fxVec2 collisionMTV(BBCollider&);
        fxVec2 collisionMTV(CGCollider&, fxVec2&);
        fxVec2 collisionMTV(CGCollider&);
    };

    struct BBCollider
    {
        fxVec2 pos; // On most coordinate systems, this represents the upper-left or lower-left corner of the box.
        fxVec2 size;

        BBCollider(void);
        BBCollider(const fxVec2&, const fxVec2&);
        BBCollider(const BBCollider&);

        fxVec2 center(); // UNIMPLEMENTED

        bool testPoint(const fxVec2&);
        bool isCollidingWith(BCCollider&);
        bool isCollidingWith(BBCollider&);
        bool isCollidingWith(CGCollider&);
        fxVec2 collisionMTV(BCCollider&, fxVec2&);
        fxVec2 collisionMTV(BCCollider&);
        fxVec2 collisionMTV(BBCollider&, fxVec2&);
        fxVec2 collisionMTV(BBCollider&);
        fxVec2 collisionMTV(CGCollider&, fxVec2&);
        fxVec2 collisionMTV(CGCollider&);
    };

    struct CGCollider
    {
        fxVec2* vs; // Polygon vertices
        fxVec2* ns; // Surface normals
        int n;      // Number of vertices

        CGCollider(void);
        CGCollider(fxVec2*, const int, const bool);
        CGCollider(fxVec2*, fxVec2*, const int, const bool); // Use this if we already have the normals
        CGCollider(const CGCollider&);
        CGCollider(const CGCollider&, const bool);

        fxVec2 center(); // UNIMPLEMENTED

        bool testPoint(fxVec2&);
        bool isCollidingWith(BCCollider&); // UNIMPLEMENTED
        bool isCollidingWith(BBCollider&);
        bool isCollidingWith(const CGCollider&); // UNIMPLEMENTED
        fxVec2 collisionMTV(BCCollider&, fxVec2&);
        fxVec2 collisionMTV(BCCollider&);
        fxVec2 collisionMTV(BBCollider&, fxVec2&);
        fxVec2 collisionMTV(BBCollider&);
        fxVec2 collisionMTV(CGCollider&, fxVec2&);
        fxVec2 collisionMTV(CGCollider&);
    };

    // Primary feldspar entity type. Basically a holder for a collider and some attributes.
    struct gameEntity // UNIMPLEMENTED
    {

      gameEntity(void);
      gameEntity(BCCollider&, const fxp_t);
      gameEntity(BBCollider&, const fxp_t);
      gameEntity(CGCollider&, const fxp_t);

      enum colliderType
      {
        boundingCircle,
        boundingBox,
        boundingPolygon
      };
      union colliderPtr
      {
        BCCollider* bc;
        BBCollider* bb;
        CGCollider* cg;
      };
      bool eStatic // Static boolean
      fxVec2 vel;
      fxVec2 com; // Center of mass
      fxp_t mass;
      void (*onTick)(gameEntity&);
      void (*onCollide)(gameEntity&, gameEntity&);
      void (*onPostTick)(gameEntity&);
    };
}

#endif // FELDSPAR_H_INCLUDED
