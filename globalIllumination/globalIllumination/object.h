#pragma once

#include <vector>
#include "definition.h"
#include "triangle.h"
#include "ray.h"

class object
{
	public:
		object(glm::vec3 p);
		~object();
		glm::vec3 getPosition();
		virtual glm::vec3 rayIntersect(ray& r) = 0;

	private:
		glm::vec3 position;
};

