
/*
    Mason Watmough's Feldspar engine, 2D vector implementation.
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

fs::fxVec2::fxVec2(void){}

fs::fxVec2::fxVec2(const fs::fxp_t _x, const fs::fxp_t _y)
{
    x = _x;
    y = _y;
}

fs::fxVec2::fxVec2(const float _x, const float _y)
{
    x = fs::f2fx(_x);
    y = fs::f2fx(_y);
}

fs::fxVec2::fxVec2(const fs::fxVec2& _rhs)
{
    x = _rhs.x;
    y = _rhs.y;
}

// 2d arithmetic

fs::fxVec2 fs::fxVec2::operator+(const fs::fxVec2& _rhs)
{
    return fs::fxVec2(fxp_t(x + _rhs.x), fxp_t(y + _rhs.y));
}

fs::fxVec2 fs::fxVec2::operator-(const fs::fxVec2& _rhs)
{
    return fs::fxVec2(fxp_t(x - _rhs.x), fxp_t(y - _rhs.y));
}

fs::fxVec2 fs::fxVec2::operator*(const fs::fxVec2& _rhs)
{
    return fs::fxVec2(fs::fxp_mul(x, _rhs.x), fs::fxp_mul(y, _rhs.y));
}

fs::fxVec2 fs::fxVec2::operator/(const fs::fxVec2& _rhs)
{
    return fs::fxVec2(fs::fxp_div(x, _rhs.x), fs::fxp_div(y, _rhs.y));
}

// Assignment-equals

fs::fxVec2 fs::fxVec2::operator+=(const fs::fxVec2& _rhs)
{
    x += _rhs.x;
    y += _rhs.y;
    return *this;
}

fs::fxVec2 fs::fxVec2::operator-=(const fs::fxVec2& _rhs)
{
    x -= _rhs.x;
    y -= _rhs.y;
    return *this;
}

fs::fxVec2 fs::fxVec2::operator*=(const fs::fxVec2& _rhs)
{
    x = fs::fxp_mul(x, _rhs.x);
    y = fs::fxp_mul(y, _rhs.y);
    return *this;
}

fs::fxVec2 fs::fxVec2::operator/=(const fs::fxVec2& _rhs)
{
    x = fs::fxp_div(x, _rhs.x);
    y = fs::fxp_div(y, _rhs.y);
    return *this;
}

// Vector-scalar arithmetic

fs::fxVec2 fs::fxVec2::operator+(const fs::fxp_t _rhs)
{
    return fs::fxVec2(fxp_t(x + _rhs), fxp_t(y + _rhs));
}

fs::fxVec2 fs::fxVec2::operator-(const fs::fxp_t _rhs)
{
    return fs::fxVec2(fxp_t(x - _rhs), fxp_t(y - _rhs));
}

fs::fxVec2 fs::fxVec2::operator*(const fs::fxp_t _rhs)
{
    return fs::fxVec2(fs::fxp_mul(x, _rhs), fs::fxp_mul(y, _rhs));
}

fs::fxVec2 fs::fxVec2::operator/(const fs::fxp_t _rhs)
{
    return fs::fxVec2(fs::fxp_div(x, _rhs), fs::fxp_div(y, _rhs));
}

// Assignment-equals

fs::fxVec2 fs::fxVec2::operator+=(const fs::fxp_t _rhs)
{
    x += _rhs;
    y += _rhs;
    return *this;
}

fs::fxVec2 fs::fxVec2::operator-=(const fs::fxp_t _rhs)
{
    x -= _rhs;
    y -= _rhs;
    return *this;
}

fs::fxVec2 fs::fxVec2::operator*=(const fs::fxp_t _rhs)
{
    x = fs::fxp_mul(x, _rhs);
    y = fs::fxp_mul(y, _rhs);
    return *this;
}

fs::fxVec2 fs::fxVec2::operator/=(const fs::fxp_t _rhs)
{
    x = fs::fxp_div(x, _rhs);
    y = fs::fxp_div(y, _rhs);
    return *this;
}

bool fs::fxVec2::operator==(const fs::fxVec2& _rhs)
{
    if(x == _rhs.x && y == _rhs.y) return true;
    return false;
}

bool fs::fxVec2::operator!=(const fs::fxVec2& _rhs)
{
    if(x != _rhs.x || y != _rhs.y) return true;
    return false;
}

bool fs::fxVec2::operator>(const fs::fxVec2& _rhs)
{
    large_t l1 = large_t(x)*x + large_t(y)*y;
    large_t l2 = large_t(_rhs.x) * _rhs.x + large_t(_rhs.y) * _rhs.y;
    if(l1 > l2) return  true;
    return false;
}

bool fs::fxVec2::operator>=(const fs::fxVec2& _rhs)
{
    large_t l1 = large_t(x)*x + large_t(y)*y;
    large_t l2 = large_t(_rhs.x) * _rhs.x + large_t(_rhs.y) * _rhs.y;
    if(l1 >= l2) return  true;
    return false;
}

bool fs::fxVec2::operator<(const fs::fxVec2& _rhs)
{
    large_t l1 = large_t(x)*x + large_t(y)*y;
    large_t l2 = large_t(_rhs.x) * _rhs.x + large_t(_rhs.y) * _rhs.y;
    if(l1 < l2) return  true;
    return false;
}

bool fs::fxVec2::operator<=(const fs::fxVec2& _rhs)
{
    large_t l1 = large_t(x)*x + large_t(y)*y;
    large_t l2 = large_t(_rhs.x) * _rhs.x + large_t(_rhs.y) * _rhs.y;
    if(l1 <= l2) return  true;
    return false;
}

fs::fxVec2 fs::fxVec2::normal()
{
    fxp_t l = fs::fxp_hypot(x, y);
    return fs::fxVec2(fs::fxp_div(x, l), fs::fxp_div(y, l));
}

fs::fxVec2 fs::fxVec2::ltan()
{
    return fs::fxVec2(fxp_t(y), -fxp_t(x));
}

fs::fxVec2 fs::fxVec2::rtan()
{
    return fs::fxVec2(-fxp_t(y), fxp_t(x));
}

fs::fxVec2 fs::fxVec2::rotation(const fxp_t _t)
{
    fxp_t _s = fs::fxp_sindeg(_t);
    fxp_t _c = fs::fxp_cosdeg(_t);
    return rotation(_s, _c);
}

fs::fxVec2 fs::fxVec2::rotation(const fxp_t _s, const fxp_t _c)
{
    //return fs::fxVec2(fs::fxp_mul(x, _c) - fs::fxp_mul(y, _s), fs::fxp_mul(x, _s) + fs::fxp_mul(y, _c));
    return fs::fxVec2((fxp_t)((large_t(x) * _c - large_t(y) * _s) >> FRACBITS), (fxp_t)((large_t(x) * _s + large_t(y) * _c) >> FRACBITS));
}

fs::fxp_t fs::fxVec2::len2()
{
    return (large_t(x)*x + large_t(y)*y) >> FRACBITS;
}

fs::fxp_t fs::fxVec2::len()
{
    return fs::fxp_hypot(x, y);
}

fs::fxp_t fs::fxVec2::dist2(const fs::fxVec2& _rhs)
{
    fs::fxVec2 d = *this - _rhs;
    return d.len2();
}

fs::fxp_t fs::fxVec2::dist(const fs::fxVec2& _rhs)
{
    fs::fxVec2 d = *this - _rhs;
    return d.len();
}

fs::fxp_t fs::fxVec2::dot(const fs::fxVec2& _rhs)
{
    return (large_t(x) * _rhs.x + large_t(y) * _rhs.y) >> FRACBITS;
}

fs::fxp_t fs::fxVec2::cross(const fs::fxVec2& _rhs)
{
    return (large_t(x) * _rhs.y - large_t(y) * _rhs.x) >> FRACBITS;
}
