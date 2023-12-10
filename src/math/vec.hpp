#pragma once

#include <cmath>
#include <cstdint>

template<class T>
struct Vec2_t
{
	Vec2_t() : x(0), y(0) {}
	Vec2_t(T x, T y) : x(x), y(y) {}
	T x;
	T y;
};

template<class T>
struct Vec3_t
{
	Vec3_t() : x(0), y(0), z(0) {}
	Vec3_t(T x, T y, T z) : x(x), y(y), z(z) {}
	T x;
	T y;
	T z;
};

using Vec2 = Vec2_t<int32_t>;
using uVec2 = Vec2_t<uint32_t>;

using Vec3 = Vec3_t<int32_t>;
using uVec3 = Vec3_t<uint32_t>;
