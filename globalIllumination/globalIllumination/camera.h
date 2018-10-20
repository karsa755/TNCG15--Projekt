#pragma once

#include <iostream>
#include <random>

#include "definition.h"
#include "pixel.h"
#include "object.h"
#include "polyModel.h"

class camera
{
public:
	camera(std::vector<object*> ol);
	~camera();

	void switchEye(glm::vec3 & e);
	triangle getLightSource();
	void getLocalCoordSystem(const glm::vec3 &Z, const glm::vec3 &I, glm::vec3 &X, glm::vec3 &Y);
	glm::vec3 localToWorld(const glm::vec3 &X, const glm::vec3 &Y, const glm::vec3 &Z, const glm::vec3 & v);
	glm::vec3 worldToLocal(const glm::vec3 &X, const glm::vec3 &Y, const glm::vec3 &Z, const glm::vec3 & v);

	std::pair<glm::vec3, std::pair<object*, triangle*>> findClosestIntersection(ray &r);
	color castRay(ray &r, int depth);

	void render();

private:
	void findLightSource();
	const static int width = 100;
	const static int height = 100;
	const glm::vec3 * currentEye;
	const glm::vec3 position = glm::vec3(0.0,0.0,0.0);
	const glm::vec3 eye1 = glm::vec3(-2.0,0.0,0.0);
	const glm::vec3 eye2 = glm::vec3(-1.0, 0.0, 0.0);
	const glm::vec3 plane[4] = { glm::vec3(0.0,-1.0,-1.0), glm::vec3(0.0,1.0,-1.0), glm::vec3(0.0,1.0,1.0), glm::vec3(0.0,-1.0,1.0) };
	pixel image[width][height] = { pixel() };
	std::vector<object*> objects;
	triangle lightSource;
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;
};

