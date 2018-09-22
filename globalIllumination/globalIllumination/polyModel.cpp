#include "polyModel.h"



polyModel::polyModel(std::vector<triangle> t, glm::vec3 p) : object(p)
{
	polyList = t;
}

glm::vec3 polyModel::rayIntersect(ray&r) {
	return glm::vec3();
}


polyModel::~polyModel()
{
}
