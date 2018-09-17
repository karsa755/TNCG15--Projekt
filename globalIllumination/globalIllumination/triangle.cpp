#include "triangle.h"



triangle::triangle(vertex& _v0, vertex& _v1, vertex& _v2, color _c) : 
	v0(_v0), v1(_v1), v2(_v2), surfaceColor(_c)
{
	normal = direction(glm::normalize(glm::cross(glm::vec3(v1/v1.w-v0/v0.w), glm::vec3(v2/v2.w-v1/v1.w))));
}


triangle::~triangle()
{
}
