#pragma once

#include <utility>

#include "definition.h"
#include "ray.h"
class triangle
{
	public:
		triangle();
		triangle(vertex& v0, vertex& v1, vertex& v2, color c);
		~triangle();
		std::pair<glm::vec3, triangle*> rayIntersection(ray &r);
		color getSurfaceColor();

	private:
		
		vertex v0, v1, v2;
		color surfaceColor;
		direction normal;
};

