#pragma once
#include "object.h"
class implicitModel :
	public object
{
public:
	implicitModel(float radius, glm::vec3 center);
	~implicitModel();
	std::pair<glm::vec3, triangle*> rayIntersect(ray& r);
	bool isImplicit();

private:
	float _radius;
	glm::vec3 _center;
};

