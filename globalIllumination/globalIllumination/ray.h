#pragma once

#include "definition.h"
class triangle;

class ray
{
public:
	ray(vertex& s, vertex& e, color& c);
	~ray();
	vertex getStartVec();
	vertex getEndVec();
private:
	vertex start, end;
	color rayColor;
	triangle *polygon;
};

