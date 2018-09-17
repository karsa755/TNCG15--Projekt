#pragma once

#include "definition.h"
#include "triangle.h"

class ray
{
public:
	ray(vertex& s, vertex& e, triangle& t, color& c);
	~ray();

private:
	vertex start, end;
	color rayColor;
	triangle polygon;
};

