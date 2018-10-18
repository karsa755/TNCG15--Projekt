#pragma once
#include "object.h"
#include <iostream>
class implicitModel :
	public object
{
public:
	implicitModel(float radius, glm::vec3 center, int prop, color c = color(0.0f, 0.0f, 1.0f));
	~implicitModel();
	std::pair<glm::vec3, triangle*> rayIntersect(ray& r);
	bool isImplicit();
	float getRadius();
	color getColor();

private:
	float _radius;
	color _color;
};

