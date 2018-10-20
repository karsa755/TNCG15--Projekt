#pragma once

#include <vector>

#include "definition.h"
#include "triangle.h"
#include "ray.h"

class object
{
	public:
		object();
		object(glm::vec3 p, int prop);
		~object();
		glm::vec3 getPosition();
		virtual std::pair<glm::vec3, triangle*> rayIntersect(ray& r) = 0;
		virtual bool isImplicit() = 0;
		int getSurfProperty();
		virtual float getRadius();
		virtual color getColor();
		virtual std::vector<triangle> getPolyList();

	private:
		glm::vec3 position;
		int surfaceProperty;
		
};

