#pragma once
#include "definition.h"

class triangle
{
	public:
		triangle(vertex& v0, vertex& v1, vertex& v2, color c);
		~triangle();

	private:
		vertex v0, v1, v2;
		color surfaceColor;
		direction normal;
};

