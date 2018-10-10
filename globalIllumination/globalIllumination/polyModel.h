#pragma once
#include "object.h"
class polyModel :
	public object
{
public:
	polyModel(std::vector<triangle> t, glm::vec3 p);
	~polyModel();

	std::pair<glm::vec3, triangle*> rayIntersect(ray& r);
	bool isImplicit();

private:
	std::vector<triangle> polyList;
};

