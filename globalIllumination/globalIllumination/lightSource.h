#pragma once
#include "object.h"
class lightSource : public object
{
	public:
	lightSource(float rad, glm::vec3 center, color c = color(1.0f, 1.0f, 1.0f));
	~lightSource();
	bool isEmitter();
	float getRadius();
	float getArea();
	bool isImplicit();
	std::pair<glm::vec3, triangle*> rayIntersect(ray& r);
	color getColor();
	glm::vec3 getNormal();

	private:
		float radius;
		color _color;
		
};

