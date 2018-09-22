#pragma once
#include "definition.h"
#include "ray.h"
class triangle
{
	public:
		triangle(vertex& v0, vertex& v1, vertex& v2, color c);
		~triangle();
		glm::vec3 rayIntersection(ray r);

	private:
		
		vertex v0, v1, v2;
		color surfaceColor;
		direction normal;
};

