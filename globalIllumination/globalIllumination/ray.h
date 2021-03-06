#pragma once

#include "definition.h"
class triangle;

class ray
{
public:
	ray(vertex& s, vertex& e);
	~ray();
	vertex getStartVec();
	vertex getEndVec();
	void setTriangle(triangle *t);
	void setColor(color &c);

private:
	vertex start, end;
	color rayColor;
	triangle *polygon;
};

