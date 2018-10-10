#include "implicitModel.h"



implicitModel::implicitModel(float radius, glm::vec3 center)
{
	_radius = radius;
	_center = center;
}

implicitModel::~implicitModel()
{
}

std::pair<glm::vec3, triangle*> implicitModel::rayIntersect(ray & r)
{
	float a, b, c, d;
	float distance;
	glm::vec3 hitpoint, normal;
	std::pair<glm::vec3, triangle*> tr(glm::vec3(-1.0f), nullptr);
	glm::vec3 startVec = r.getStartVec();
	glm::vec3 endVec = r.getEndVec();
	glm::vec3 dir = endVec - startVec;
	a = glm::dot(dir, dir);
	b = glm::dot(dir,2.0f * (startVec - _center));
	c = glm::dot(_center, _center) + glm::dot(startVec, startVec) - 2.0f * glm::dot(startVec, _center) - _radius * _radius;
	d = b * b + (-4.0f)*a*c;
	if (d < 0)
	{
		return tr;
	}
	float t = (-0.5f)*(b + d) / a;
	if (t > 0.0f)
	{
		distance = sqrtf(a)*t;
		hitpoint = startVec + t * dir;
		normal = (hitpoint - _center) / _radius;
		tr.first = hitpoint;
		return tr;
	}
}

bool implicitModel::isImplicit()
{
	return true;
}
