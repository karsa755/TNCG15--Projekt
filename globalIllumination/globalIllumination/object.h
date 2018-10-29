#pragma once

#include <vector>

#include "definition.h"
#include "triangle.h"
#include "ray.h"

class object
{
	public:
		object();
		object(glm::vec3 p, int prop, double r);
		~object();
		glm::vec3 getPosition();
		virtual std::pair<glm::vec3, triangle*> rayIntersect(ray& r) = 0;
		virtual bool isImplicit() = 0;
		int getSurfProperty();
		virtual float getRadius();
		virtual color getColor();
		virtual std::vector<triangle> getPolyList();
		double getRho();
		virtual bool isEmitter() = 0;

	private:
		glm::vec3 position;
		int surfaceProperty;
		double rho;
		
};

