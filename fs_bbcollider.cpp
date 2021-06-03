/*
    Mason Watmough's Feldspar engine, bounding box collider implementation.
    Copyright (C) 2021 Mason Watmough

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "feldspar.h"

extern fs::fxVec2 dummy;

fs::BBCollider::BBCollider(void)
{

}

fs::BBCollider::BBCollider(const fs::fxVec2& _p, const fs::fxVec2& _s)
{
    pos = _p;
    size = _s;
}

fs::BBCollider::BBCollider(const fs::BBCollider& _c)
{
    pos = _c.pos;
    size = _c.size;
}

bool fs::BBCollider::testPoint(const fs::fxVec2& _p)
{
    if(_p.x < pos.x) return false;
    if(_p.x > pos.x + size.x) return false;
    if(_p.y < pos.y) return false;
    if(_p.y > pos.y + size.y) return false;
    return true;
}

bool fs::BBCollider::isCollidingWith(fs::BBCollider& _other)
{
    if(pos.x > _other.pos.x + _other.size.x) return false;
    if(pos.x + size.x < _other.pos.x) return false;
    if(pos.y > _other.pos.y + _other.size.y) return false;
    if(pos.x + size.y < _other.pos.y) return false;
    return true;
}

bool fs::BBCollider::isCollidingWith(fs::BCCollider& _other)
{
    return _other.isCollidingWith(*this);
}

fs::fxVec2 fs::BBCollider::collisionMTV(fs::BBCollider& _other, fs::fxVec2& _normal)
{
    fs::fxp_t lp = pos.x + size.x - _other.pos.x;
    if(lp < 0)
    {
        _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
        return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
    }
    fs::fxp_t rp = _other.pos.x + _other.size.x - pos.x;
    if(rp < 0)
    {
        _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
        return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
    }
    fs::fxp_t up = pos.y + size.y - _other.pos.y;
    if(up < 0)
    {
        _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
        return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
    }
    fs::fxp_t dp = _other.pos.y + _other.size.y - pos.y;
    if(dp < 0)
    {
        _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
        return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
    }
    fs::fxp_t mp = fs::fxp_min(lp, fs::fxp_min(rp, fs::fxp_min(up, dp)));
    if(lp == mp)
    {
        _normal = fs::fxVec2(fs::f2fx(-1.0f), fs::f2fx(0.0f));
        return fs::fxVec2(-lp, 0);
    }
    else if(rp == mp)
    {
        _normal = fs::fxVec2(fs::f2fx(1.0f), fs::f2fx(0.0f));
        return fs::fxVec2(rp, 0);
    }
    else if(up == mp)
    {
        _normal = fs::fxVec2(fs::f2fx(0.0f), fs::f2fx(-1.0f));
        return fs::fxVec2(0, -up);
    }
    else
    {
        _normal = fs::fxVec2(fs::f2fx(0.0f), fs::f2fx(1.0f));
        return fs::fxVec2(0, dp);
    }
}

fs::fxVec2 fs::BBCollider::collisionMTV(fs::BBCollider& _other)
{
    return collisionMTV(_other, dummy);
}

fs::fxVec2 fs::BBCollider::collisionMTV(fs::BCCollider& _other, fs::fxVec2& _normal)
{
    fs::fxVec2 mtv = _other.collisionMTV(*this, _normal);
    _normal.x = -_normal.x;
    _normal.y = -_normal.y;
    mtv.x = -mtv.x;
    mtv.y = -mtv.y;
    return mtv;
}

fs::fxVec2 fs::BBCollider::collisionMTV(fs::BCCollider& _other)
{
    return collisionMTV(_other, dummy);
}

fs::fxVec2 fs::BBCollider::collisionMTV(fs::CGCollider& _other, fs::fxVec2& _normal)
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
        // Project us onto the axis...
        fs::fxp_t p = pos.dot(_other.ns[i]);
        if(p < bMin) bMin = p;
        if(p > bMax) bMax = p;
        p = fs::fxVec2(pos.x + size.x, pos.y).dot(_other.ns[i]);
        if(p < bMin) bMin = p;
        if(p > bMax) bMax = p;
        p = fs::fxVec2(pos.x + size.x, pos.y + size.y).dot(_other.ns[i]);
        if(p < bMin) bMin = p;
        if(p > bMax) bMax = p;
        p = fs::fxVec2(pos.x, pos.y + size.y).dot(_other.ns[i]);
        if(p < bMin) bMin = p;
        if(p > bMax) bMax = p;
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
    // Now for each of our normals...
    aMin = 0x7FFFFFFF;
    aMax = 0x80000000;
    bMin = 0x7FFFFFFF;
    bMax = 0x80000000;
    for(int j = 0; j < _other.n; j++)
    {
        // Project the polygon's vertices onto the normal as an axis...
        fs::fxp_t p = -_other.vs[j].y;
        if(p < aMin) aMin = p;
        if(p > aMax) aMax = p;
    }
    bMax = -pos.y;
    bMin = -(pos.y + size.y);
    if(aMax < bMin || bMax < aMin)
    {
        // Separating axis!
        _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
        return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
    }
    if(aMax - bMin < pDist)
    {
        pDist = aMax - bMin;
        _normal = fs::fxVec2(fs::f2fx(0.0f), fs::f2fx(-1.0f));
    }
    aMin = 0x7FFFFFFF;
    aMax = 0x80000000;
    bMin = 0x7FFFFFFF;
    bMax = 0x80000000;
    for(int j = 0; j < _other.n; j++)
    {
        // Project the polygon's vertices onto the normal as an axis...
        fs::fxp_t p = _other.vs[j].y;
        if(p < aMin) aMin = p;
        if(p > aMax) aMax = p;
    }
    bMin = pos.y;
    bMax = pos.y + size.y;
    if(aMax < bMin || bMax < aMin)
    {
        // Separating axis!
        _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
        return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
    }
    if(aMax - bMin < pDist)
    {
        pDist = aMax - bMin;
        _normal = fs::fxVec2(fs::f2fx(0.0f), fs::f2fx(1.0f));
    }
    aMin = 0x7FFFFFFF;
    aMax = 0x80000000;
    bMin = 0x7FFFFFFF;
    bMax = 0x80000000;
    for(int j = 0; j < _other.n; j++)
    {
        // Project the polygon's vertices onto the normal as an axis...
        fs::fxp_t p = -_other.vs[j].x;
        if(p < aMin) aMin = p;
        if(p > aMax) aMax = p;
    }
    bMax = -pos.x;
    bMin = -(pos.x + size.x);
    if(aMax < bMin || bMax < aMin)
    {
        // Separating axis!
        _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
        return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
    }
    if(aMax - bMin < pDist)
    {
        pDist = aMax - bMin;
        _normal = fs::fxVec2(fs::f2fx(-1.0f), fs::f2fx(0.0f));
    }
    aMin = 0x7FFFFFFF;
    aMax = 0x80000000;
    bMin = 0x7FFFFFFF;
    bMax = 0x80000000;
    for(int j = 0; j < _other.n; j++)
    {
        // Project the polygon's vertices onto the normal as an axis...
        fs::fxp_t p = _other.vs[j].x;
        if(p < aMin) aMin = p;
        if(p > aMax) aMax = p;
    }
    bMin = pos.x;
    bMax = pos.x + size.x;
    if(aMax < bMin || bMax < aMin)
    {
        // Separating axis!
        _normal = fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
        return fs::fxVec2((fs::fxp_t)0, (fs::fxp_t)0);
    }
    if(aMax - bMin < pDist)
    {
        pDist = aMax - bMin;
        _normal = fs::fxVec2(fs::f2fx(1.0f), fs::f2fx(0.0f));
    }
    return _normal * pDist;
}

fs::fxVec2 fs::BBCollider::collisionMTV(fs::CGCollider& _other)
{
    return collisionMTV(_other, dummy);
}
