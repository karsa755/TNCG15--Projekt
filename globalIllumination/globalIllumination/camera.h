#pragma once

#include "definition.h"
#include "pixel.h"

class camera
{
public:
	camera();
	~camera();

	void switchEye(glm::vec3 & e);
	void render();

private:
	const static int width = 800;
	const static int height = 800;
	const glm::vec3 * currentEye;
	const glm::vec3 position = glm::vec3(0.0,0.0,0.0);
	const glm::vec3 eye1 = glm::vec3(-2.0,0.0,0.0);
	const glm::vec3 eye2 = glm::vec3(-1.0, 0.0, 0.0);
	const glm::vec3 plane[4] = { glm::vec3(0.0,-1.0,-1.0), glm::vec3(0.0,1.0,-1.0), glm::vec3(0.0,1.0,1.0), glm::vec3(0.0,-1.0,1.0) };
	pixel image[width][height] = { pixel() };
};

