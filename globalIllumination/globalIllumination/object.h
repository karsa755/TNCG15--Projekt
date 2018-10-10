#pragma once

#include <vector>

#include "definition.h"
#include "triangle.h"
#include "ray.h"

class object
{
	public:
		object();
		object(glm::vec3 p);
		~object();
		glm::vec3 getPosition();
		virtual std::pair<glm::vec3, triangle*> rayIntersect(ray& r) = 0;
		virtual bool isImplicit() = 0;

	private:
		glm::vec3 position;
		
};

