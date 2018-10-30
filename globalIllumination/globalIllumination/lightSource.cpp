#include "lightSource.h"



lightSource::lightSource(float rad, glm::vec3 center, color c): object(center, 0, 0.0), radius(rad), _color(c)
{
}

lightSource::~lightSource()
{

}

bool lightSource::isEmitter()
{
	return true;
}

float lightSource::getRadius()
{
	return radius;
}

float lightSource::getArea()
{
	return (float)PI*radius*radius;
}

bool lightSource::isImplicit()
{
	return true;
}

glm::vec2 lightSource::sampleCircle(float u, float v)
{
	glm::vec3 pos = getPosition();
	float phi = 2.0f * PI * u;
	float r = std::sqrtf(v) * radius;
	float x = r * cosf(phi) + pos.x;
	float y = r * sinf(phi) + pos.y;
	return glm::vec2(x, y);
	
}

std::pair<glm::vec3, triangle*> lightSource::rayIntersect(ray & r)
{
	std::pair<glm::vec3, triangle*> tr(glm::vec3(MAX_FLOAT), nullptr); //should be numeric limits instead of -1.
	glm::vec3 startVec = r.getStartVec();
	glm::vec3 endVec = r.getEndVec();
	glm::vec3 dir = glm::normalize(endVec - startVec);
	glm::vec3 normal = getNormal();
	float dist = 0.0f;
	bool intersection = glm::intersectRayPlane(startVec, dir, getPosition(), normal, dist);
	if (intersection)
	{
		glm::vec3 pt = startVec + dist * dir;
		float intersectDist = glm::distance(pt, getPosition());
		if (intersectDist < radius)
		{
			tr.first = pt;
			return tr;
		} 
		else
		{
			return tr;
		}
	}
	else
	{
		return tr;
	}

}

color lightSource::getColor()
{
	return _color;
}

glm::vec3 lightSource::getNormal()
{
	return glm::vec3(0.0f, 0.0f, -1.0f);
}
