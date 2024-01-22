#pragma once

#include <cmath>

template <class t> struct Vec2
{
    t x, y;
};

template <class t> struct Vec3
{
    t x, y, z;
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

inline Vec2i WorldToScreenSpace(Vec3f v, int width, int height) {
	Vec2i pixel;
	pixel.x = (int) ((v.x + 1.0) * width / 2.0);
	pixel.y = (int) ((v.y + 1.0) * height / 2.0);

	return pixel;
}