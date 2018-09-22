#include "polyModel.h"



polyModel::polyModel(std::vector<triangle> t, glm::vec3 p) : object(p)
{
	polyList = t;
}

glm::vec3 polyModel::rayIntersect(ray& r) {
	for (auto it = begin(polyList); it != end(polyList); ++it) {
		if (it->rayIntersection(r) != glm::vec3(-1.0, -1.0, -1.0))
		{
			return it->rayIntersection(r);
		}
	}
	//nothing was found.
	return glm::vec3(-1.0, -1.0, -1.0);
}


polyModel::~polyModel()
{
}
