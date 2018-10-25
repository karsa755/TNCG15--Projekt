#include "implicitModel.h"



implicitModel::implicitModel(float radius, glm::vec3 center, int prop, double r, color c): object(center, prop, r), _color(c), _radius(radius)
{	
}


implicitModel::~implicitModel()
{
}

std::pair<glm::vec3, triangle*> implicitModel::rayIntersect(ray & r)
{
	//float a, b, c, d;
	//float distance;
	//glm::vec3 hitpoint, normal;
	std::pair<glm::vec3, triangle*> tr(glm::vec3(MAX_FLOAT), nullptr); //should be numeric limits instead of -1.
	glm::vec3 startVec = r.getStartVec();
	glm::vec3 endVec = r.getEndVec();
	glm::vec3 dir = glm::normalize(endVec - startVec);
	//a = glm::dot(dir, dir);
	//b = glm::dot(dir,2.0f * (startVec - getPosition()));
	//c = glm::dot(getPosition(), getPosition()) + glm::dot(startVec, startVec) - 2.0f * glm::dot(startVec, getPosition()) - _radius * _radius;
	//d = b * b + (-4.0f)*a*c;
	//if (d < 0)
	//{
	//	return tr;
	//}
	//d = std::sqrt(d);
	//std::cout << "";
	//float t = (-0.5f)*(b + d) / a;
	//if (t > 0.0f)
	//{
	//	distance = sqrtf(a)*t;
	//	hitpoint = startVec + t * dir;
	//	//normal = (hitpoint - getPosition()) / _radius;
	//	//std::cout << " implicit ";
	//	tr.first = hitpoint;
	//	return tr;
	//}
	glm::vec3 normalNew(0.0f), point(0.0f);
	bool intersect = glm::intersectRaySphere(startVec, dir, getPosition(), _radius, point, normalNew);
	if (intersect)
	{
		tr.first = point;
		tr.second = r.bogusTriangle;
		return tr;
	}
	else
	{
		return tr;
	}
}

bool implicitModel::isImplicit()
{
	return true;
}

float implicitModel::getRadius()
{
	return _radius;
}

color implicitModel::getColor()
{
	return _color;
}

