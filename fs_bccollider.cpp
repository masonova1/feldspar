#include "feldspar.h"

//Exactly what it sounds like. Helps with default arguments.
fs::fxVec2 dummy;

fs::BCCollider::BCCollider(void)
{

}

fs::BCCollider::BCCollider(const fs::fxVec2& _pos, const fxp_t _r)
{
    pos = _pos;
    r = _r;
}

fs::BCCollider::BCCollider(const fs::BCCollider& _c)
{
    pos = _c.pos;
    r = _c.r;
}

bool fs::BCCollider::testPoint(const fs::fxVec2& _p)
{
    int64_t dx2 = pos.x - _p.x;
    dx2 *= dx2;
    int64_t dy2 = pos.y - _p.y;
    dy2 *= dy2;
    int64_t r2 = int64_t(r) * r;
    if(dx2 + dy2 <= r2) return true;
    return false;
}

bool fs::BCCollider::isCollidingWith(fs::BCCollider& _other)
{
    int64_t dx2 = pos.x - _other.pos.x;
    dx2 *= dx2;
    int64_t dy2 = pos.y - _other.pos.y;
    dy2 *= dy2;
    int64_t r2 = r + _other.r;
    r2 *= r2;
    if(dx2 + dy2 <= r2) return true;
    return false;
}

fs::fxVec2 fs::BCCollider::collisionMTV(fs::BCCollider& _other, fs::fxVec2& _normal)
{
    int64_t dx2 = pos.x - _other.pos.x;
    dx2 *= dx2;
    int64_t dy2 = pos.y - _other.pos.y;
    dy2 *= dy2;
    int64_t r2 = r + _other.r;
    r2 *= r2;
    if(dx2 + dy2 > r2 || (dx2 == 0 && dy2 == 0))
    {
        // Circles cannot possibly be colliding, or there is no MTV.
        // Identical centers are return cases to avoid divisions by zero.
        _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
        return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
    }
    int64_t s = (dx2+dy2)>>16;
    int scale = 0;
    // Copy of hypot overflow protection code because we already know distance^2
    while(s > 0x7FFFFFFF)
    {
        s >>= 2;
        scale++;
    }
    fs::fxp_t dist = fs::fxp_sqrt(s) << scale;
    _normal = (pos - _other.pos) / dist;
    return ((pos-_other.pos) * fs::fxp_div(r + _other.r, dist)) - (pos - _other.pos);
}

fs::fxVec2 fs::BCCollider::collisionMTV(fs::BCCollider& _other)
{
    return collisionMTV(_other, dummy);
}

bool fs::BCCollider::isCollidingWith(fs::BBCollider& _other)
{
    fs::fxp_t tx = pos.x;
    fs::fxp_t ty = pos.y;
    if(pos.x < _other.pos.x) tx = _other.pos.x;
    else if(pos.x > _other.pos.x + _other.size.x) tx = _other.pos.x + _other.size.x;
    if(pos.y < _other.pos.y) ty = _other.pos.y;
    else if(pos.y > _other.pos.y + _other.size.y) ty = _other.pos.y + _other.size.y;
    int64_t r2 = int64_t(r) * r;
    int64_t dx2 = pos.x - tx;
    dx2 *= dx2;
    int64_t dy2 = pos.y - ty;
    dy2 *= dy2;
    if(dx2 + dy2 <= r2) return true;
    return false;
}

fs::fxVec2 fs::BCCollider::collisionMTV(fs::BBCollider& _other, fs::fxVec2& _normal)
{
    fs::fxVec2 test = pos;
    if(pos.x < _other.pos.x) test.x = _other.pos.x;
    else if(pos.x > _other.pos.x + _other.size.x) test.x = _other.pos.x + _other.size.x;
    if(pos.y < _other.pos.y) test.y = _other.pos.y;
    else if(pos.y > _other.pos.y + _other.size.y) test.y = _other.pos.y + _other.size.y;
    int64_t r2 = int64_t(r) * r;
    int64_t dx2 = pos.x - test.x;
    dx2 *= dx2;
    int64_t dy2 = pos.y - test.y;
    dy2 *= dy2;
    int64_t s = (dx2+dy2);
    if(s > r2)
    {
        // Circle and box can't possibly be intersecting
        _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
        return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
    }
    if(s == 0)
    {
        // Circle center is inside box
        fxp_t lp = pos.x - _other.pos.x;
        fxp_t rp = _other.pos.x + _other.size.x - pos.x;
        fxp_t up = pos.y - _other.pos.y;
        fxp_t dp = _other.pos.y + _other.size.y - pos.y;
        fxp_t mp = fs::fxp_min(lp, fs::fxp_min(rp, fs::fxp_min(up, dp)));
        if(lp == mp)
        {
            _normal = fs::fxVec2(fs::f2fx(-1.0), fs::f2fx(0.0));
            return fs::fxVec2(-lp-r, 0);
        }
        else if(rp == mp)
        {
            _normal = fs::fxVec2(fs::f2fx(1.0), fs::f2fx(0.0));
            return fs::fxVec2(rp+r, 0);
        }
        else if(up == mp)
        {
            _normal = fs::fxVec2(fs::f2fx(0.0), fs::f2fx(-1.0));
            return fs::fxVec2(0, -up-r);
        }
        else
        {
            _normal = fs::fxVec2(fs::f2fx(0.0), fs::f2fx(1.0));
            return fs::fxVec2(0, dp+r);
        }
    }
    s >>= 16;
    int scale = 0;
    // Copy of hypot overflow protection code because we already know distance^2
    while(s > 0x7FFFFFFF)
    {
        s >>= 2;
        scale++;
    }
    fs::fxp_t dist = fs::fxp_sqrt(s) << scale;
    _normal = (pos - test) / dist;
    return ((pos-test) * fs::fxp_div(r, dist)) - (pos - test);
}

fs::fxVec2 fs::BCCollider::collisionMTV(fs::BBCollider& _other)
{
    return collisionMTV(_other, dummy);
}

fs::fxVec2 fs::BCCollider::collisionMTV(fs::CGCollider& _other, fs::fxVec2& _normal)
{
    fs::fxp_t aMin, aMax, bMin, bMax;
    fs::fxp_t pDist = 0x7FFFFFFF;
    for(int i = 0; i < _other.n; i++)
    {
        // For each of the polygon's normals...
        aMin = 0x7FFFFFFF;
        aMax = 0x80000000;
        bMin = 0x7FFFFFFF;
        bMax = 0x80000000;
        for(int j = 0; j < _other.n; j++)
        {
            // Project the polygon's vertices onto the normal as an axis...
            fs::fxp_t p = _other.vs[j].dot(_other.ns[i]);
            if(p < aMin) aMin = p;
            if(p > aMax) aMax = p;
        }
        //Project the circle onto the normal as an axis...
        bMin = bMax = pos.dot(_other.ns[i]);
        bMin -= r;
        bMax += r;
        if(aMax < bMin || bMax < aMin)
        {
            // Separating axis!
            _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
            return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
        }
        if(aMax - bMin < pDist)
        {
            pDist = aMax - bMin;
            _normal = _other.ns[i];
        }
    }
    for(int i = 0; i < _other.n; i++)
    {
        // Now check using the axes between the circle's center and the vertices
        aMin = 0x7FFFFFFF;
        aMax = 0x80000000;
        bMin = 0x7FFFFFFF;
        bMax = 0x80000000;
        fs::fxVec2 a = (pos-_other.vs[i]);
        if(a.x != 0 || a.y != 0)
        {
            a = a.normal();
            for(int j = 0; j < _other.n; j++)
            {
                fs::fxp_t p = _other.vs[j].dot(a);
                if(p < aMin) aMin = p;
                if(p > aMax) aMax = p;
            }
            bMin = bMax = pos.dot(a);
            bMin -= r;
            bMax += r;
            if(aMax < bMin || bMax < aMin)
            {
                // Separating axis!
                _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
                return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
            }
            if((aMax - bMin) < pDist)
            {
                pDist = (aMax - bMin);
                _normal = a;
            }
        }
    }
    return _normal * pDist;
}

fs::fxVec2 fs::BCCollider::collisionMTV(fs::CGCollider& _other)
{
    return collisionMTV(_other, dummy);
}
