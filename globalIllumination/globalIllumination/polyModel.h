#pragma once
#include "object.h"
class polyModel :
	public object
{
public:
	polyModel(std::vector<triangle> t, glm::vec3 p, int prop, double r);
	~polyModel();

	std::pair<glm::vec3, triangle*> rayIntersect(ray& r);
	std::vector<triangle> getPolyList();
	bool isImplicit();
	bool isEmitter();

private:
	std::vector<triangle> polyList;
};

