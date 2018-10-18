#include "triangle.h"

#define eps std::numeric_limits<float>::epsilon()

triangle::triangle() {

}

triangle::triangle(vertex& _v0, vertex& _v1, vertex& _v2, color _c, bool emitter):
	v0(_v0), v1(_v1), v2(_v2), surfaceColor(_c), isEmitter(emitter)
{
	normal = direction(glm::normalize(glm::cross(glm::vec3(v1/v1.w-v0/v0.w), glm::vec3(v2/v2.w-v1/v1.w))));
}



triangle::~triangle()
{
}

std::pair<glm::vec3, triangle*> triangle::rayIntersection(ray &r)
{
	//can be nice to test how it makes a vec4 to a vec3..
	vertex start = r.getStartVec();
	vertex end = r.getEndVec();
	glm::vec3 T = start - v0;
	glm::vec3 E1 = v1 - v0;
	glm::vec3 E2 = v2 - v0;
	glm::vec3 D = end - start;
	glm::vec3 P = glm::cross(D, E2);
	glm::vec3 Q = glm::cross(T, E1);

	float constant = 1.0 / (glm::dot(P, E1));
	glm::vec3 result = constant * glm::vec3(glm::dot(Q, E2), glm::dot(P, T), glm::dot(Q, D));

	if (result.y < (0.0-eps) || result.z < (0.0-eps) || (result.y + result.z) > (1.0+eps))
	{
		//return "nollvektor" if there is no intersection
		//return glm::vec3(-1.0, -1.0, -1.0);
		return std::pair<glm::vec3, triangle*>(glm::vec3(-1.0), nullptr);
	}
	if (result.x > 0.0)
	{
		glm::vec3 rayStart = glm::vec3(start.x, start.y, start.z);
		//there is an intersection
		//return rayStart + D * result.x;
		return std::pair<glm::vec3, triangle*>(rayStart + D * result.x, this);
	}
	else
	{
		return std::pair<glm::vec3, triangle*>(glm::vec3(-1.0), nullptr);
	}
	
}

color triangle::getSurfaceColor() {
	return surfaceColor;
}

bool triangle::isEmitting()
{
	return isEmitter;
}

glm::vec3 triangle::getNormal()
{

	return normal;
}

float triangle::getArea()
{
	float dafq = 0.5f * glm::length(glm::cross(glm::vec3(v1 / v1.w - v0 / v0.w), glm::vec3(v2 / v2.w - v1 / v1.w)));
	return dafq;
}
