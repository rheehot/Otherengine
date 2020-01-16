#pragma once

#include "NEG.h"

NEG_BEGIN

enum e_no_init
{
	no_init
};

struct vector2
{
	float x, y;

	vector2() :x{}, y{} {}
	explicit vector2(e_no_init) {}
	vector2(const float x, const float y) :x{x}, y{y} {}

	vector2& operator+=(const vector2& a)
	{
		x += a.x;
		y += a.y;
		return *this;
	}

	vector2 operator*(const float f) const
	{
		return {x*f, y*f};
	}
};

NEG_END
