#include "polyModel.h"



polyModel::polyModel(std::vector<triangle> t, glm::vec3 p, int prop, double r) : object(p, prop,r)
{
	polyList = t;
}

std::pair<glm::vec3, triangle*> polyModel::rayIntersect(ray& r) {
	bool fuck = false;
	float smallestDist = std::numeric_limits<float>::max();
	std::pair<glm::vec3, triangle*> tr(glm::vec3(std::numeric_limits<float>::max()), nullptr);
	for (auto it = begin(polyList); it != end(polyList); ++it) {
		std::pair<glm::vec3, triangle*> temp;
		temp = it->rayIntersection(r);
		float dist = glm::distance(temp.first, (glm::vec3) r.getStartVec());
		if (temp.second != nullptr && dist < smallestDist) {
			smallestDist = dist;
			tr = temp;
			fuck = true;
		}
	}

	return (fuck) ? tr : std::pair<glm::vec3, triangle*>(glm::vec3(-1.0), nullptr);
}

std::vector<triangle> polyModel::getPolyList()
{
	return polyList;
}

bool polyModel::isImplicit()
{
	return false;
}


polyModel::~polyModel()
{
}
