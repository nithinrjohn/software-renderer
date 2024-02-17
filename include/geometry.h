#pragma once

#include <cmath>
#include <vector>

using namespace std;

template <class t> struct Vec2
{
    t x, y;

	Vec2<t> operator+(const Vec2<t>& v) const {
		return Vec2<t>{x + v.x, y + v.y};
	}

	Vec2<t> operator-(const Vec2<t>& v) const {
		return Vec2<t>{x - v.x, y - v.y};
	}

	Vec2<t> operator*(const Vec2<t>& v) const {
		return Vec2<t>{x * v.x, y * v.y};
	}

	Vec2<t> operator*(const t& s) const {
		return Vec2<t>{x * s, y * s};
	}

	Vec2<t> operator/(const Vec2<t>& v) const {
		return Vec2<t>{x / v.x, y / v.y};
	}

	Vec2<t> operator/(const t& s) const {
		return Vec2<t>{x / s, y / s};
	}
};

template <class t> struct Vec3
{
    t x, y, z;

	Vec3<t> operator+(const Vec3<t>& v) const {
		return Vec3<t>{x + v.x, y + v.y, z + v.z};
	}

	Vec3<t> operator-(const Vec3<t>& v) const {
		return Vec3<t>{x - v.x, y - v.y, z - v.z};
	}

	Vec3<t> operator*(const Vec3<t>& v) const {
		return Vec3<t>{x * v.x, y * v.y, z * v.z};
	}

	Vec3<t> operator*(const t& s) const {
		return Vec3<t>{x * s, y * s, z * s};
	}

	Vec3<t> operator/(const Vec3<t>& v) const {
		return Vec3<t>{x / v.x, y / v.y, z / v.z};
	}

	Vec3<t> operator/(const t& s) const {
		return Vec3<t>{x / s, y / s, z / s};
	}
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

inline Vec3f WorldToScreenSpace(Vec3f v, int width, int height) {
	Vec3f pixel;
	pixel.x = (int) ((v.x + 1.0) * width / 2.0);
	pixel.y = (int) ((v.y + 1.0) * height / 2.0); 
	pixel.z = v.z;

	return pixel;
}

inline Vec3f cross(Vec3f v1, Vec3f v2) {
	return Vec3f{v1.y * v2.z - v1.z * v2.y,
				 v1.z * v2.x - v1.x * v2.z,
				 v1.x * v2.y - v1.y * v2.x};
}

inline float dot(Vec3f v1, Vec3f v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vec3f barycentricCoords(vector<Vec3f> tri, Vec3f p) {
	Vec3f u = cross(Vec3f{tri[2].x - tri[0].x, tri[1].x - tri[0].x, tri[0].x - p.x}, Vec3f{tri[2].y - tri[0].y, tri[1].y - tri[0].y, tri[0].y - p.y});
	if (abs(u.z) < 1) return Vec3f{-1, 1, 1};
	return Vec3f{1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z};
}