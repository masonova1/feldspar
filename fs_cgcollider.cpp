#include "feldspar.h"

extern fs::fxVec2 dummy;

fs::CGCollider::CGCollider(void)
{

}

fs::CGCollider::CGCollider(fs::fxVec2* _vptr, const int _n, const bool _allocNew = false)
{
    n = _n;
    if(!_allocNew) vs = _vptr;
    else
    {
        vs = new fs::fxVec2[n];
        for(int i = 0; i < n; i++)
        {
            vs[i] = _vptr[i];
        }
    }
    //Generate new surface normals
    ns = new fs::fxVec2[n];
    for(int i = 0; i < n - 1; i++)
    {
        ns[i] = (vs[i + 1] - vs[i]).ltan().normal();
    }
    ns[n-1] = (vs[0] - vs[n-1]).ltan().normal();
}

fs::CGCollider::CGCollider(fs::fxVec2* _vptr, fs::fxVec2* _nptr, const int _n, const bool _allocNew = false)
{
    n = _n;
    if(!_allocNew)
    {
        vs = _vptr;
        ns = _nptr;
    }
    else
    {
        vs = new fs::fxVec2[n];
        ns = new fs::fxVec2[n];
        for(int i = 0; i < n; i++)
        {
            vs[i] = _vptr[i];
            ns[i] = _nptr[i];
        }
    }
}

fs::CGCollider::CGCollider(const fs::CGCollider& _c)
{
    vs = _c.vs;
    ns = _c.ns;
    n = _c.n;
}

fs::CGCollider::CGCollider(const fs::CGCollider& _c, const bool _allocNew = false)
{
    if(!_allocNew)
    {
        vs = _c.vs;
        ns = _c.ns;
        n = _c.n;
    }
    else
    {
        n = _c.n;
        for(int i = 0; i < n; i++)
        {
            vs[i] = _c.vs[i];
            ns[i] = _c.ns[i];
        }
    }
}

bool fs::CGCollider::testPoint(fs::fxVec2& _p)
{
    for(int i = 0; i < n; i++)
    {
        if((_p - vs[i]).dot(ns[i]) > 0) return false;
    }
    return true;
}

bool fs::CGCollider::isCollidingWith(fs::BCCollider& _other)
{
    fs::fxVec2 test;
    for(int i = 0; i < n; i++)
    {
        test = _other.pos - (ns[i] * _other.r);
        if(testPoint(test)) return true;
        if(_other.testPoint(vs[i])) return true;
    }
    return false;
}

bool fs::CGCollider::isCollidingWith(fs::BBCollider& _other)
{
    fs::fxVec2 v2(_other.pos.x + _other.size.x, _other.pos.y);
    fs::fxVec2 v3(_other.pos.x + _other.size.x, _other.pos.y + _other.size.y);
    fs::fxVec2 v4(_other.pos.x, _other.pos.y + _other.size.y);
    if(testPoint(_other.pos) || testPoint(v2) || testPoint(v3) || testPoint(v4)) return true;
    for(int i = 0; i < n; i++)
    {
        if(_other.testPoint(vs[i])) return true;
    }
    return false;
}

fs::fxVec2 fs::CGCollider::collisionMTV(fs::BCCollider& _other, fs::fxVec2& _normal)
{
    fs::fxVec2 mtv = _other.collisionMTV(*this, _normal);
    _normal.x = -_normal.x;
    _normal.y = -_normal.y;
    mtv.x = -mtv.x;
    mtv.y = -mtv.y;
    return mtv;
}

fs::fxVec2 fs::CGCollider::collisionMTV(fs::BCCollider& _other)
{
    return collisionMTV(_other, dummy);
}

fs::fxVec2 fs::CGCollider::collisionMTV(fs::CGCollider& _other, fs::fxVec2& _normal)
{
    fs::fxp_t aMin, aMax, bMin, bMax;
    fs::fxp_t pDist = 0x7FFFFFFF;
    for(int i = 0; i < n; i++)
    {
        // For each of this polygon's normals...
        aMin = 0x7FFFFFFF;
        aMax = 0x80000000;
        bMin = 0x7FFFFFFF;
        bMax = 0x80000000;
        for(int j = 0; j < n; j++)
        {
            // Project this polygon's vertices onto the normal as an axis...
            fs::fxp_t p = vs[j].dot(ns[i]);
            if(p < aMin) aMin = p;
            if(p > aMax) aMax = p;
        }
        // Project the other polygon's vertices...
        for(int j = 0; j < _other.n; j++)
        {
            fs::fxp_t p = _other.vs[j].dot(ns[i]);
            if(p < bMin) bMin = p;
            if(p > bMax) bMax = p;
        }
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
        // For each of that polygon's normals...
        aMin = 0x7FFFFFFF;
        aMax = 0x80000000;
        bMin = 0x7FFFFFFF;
        bMax = 0x80000000;
        for(int j = 0; j < n; j++)
        {
            // Project this polygon's vertices onto the normal as an axis...
            fs::fxp_t p = vs[j].dot(_other.ns[i]);
            if(p < aMin) aMin = p;
            if(p > aMax) aMax = p;
        }
        // Project the other polygon's vertices...
        for(int j = 0; j < _other.n; j++)
        {
            fs::fxp_t p = _other.vs[j].dot(_other.ns[i]);
            if(p < bMin) bMin = p;
            if(p > bMax) bMax = p;
        }
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
    return _normal * pDist;
}
