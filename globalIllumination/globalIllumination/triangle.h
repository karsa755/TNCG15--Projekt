#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include "definition.h"
#include "ray.h"
class triangle
{
	public:
		triangle();
		triangle(vertex& v0, vertex& v1, vertex& v2, double r, color c,bool emitter = false);
		~triangle();
		std::pair<glm::vec3, triangle*> rayIntersection(ray &r);
		color getSurfaceColor();
		bool isEmitting();
		glm::vec3 getNormal();
		bool isEmitter;
		float getArea();
		vertex getMidPoint();
		glm::vec3 sampleTriangle(float u, float v);
		double getRho();

	private:
		vertex v0, v1, v2;
		color surfaceColor;
		direction normal;
		float area;
		double rho;
};

