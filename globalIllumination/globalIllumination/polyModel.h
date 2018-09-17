#pragma once
#include "object.h"
class polyModel :
	public object
{
public:
	polyModel(std::vector<triangle> t, glm::vec3 p);
	~polyModel();

	glm::vec3 rayIntersect(ray& r);

private:
	std::vector<triangle> polyList;
};

