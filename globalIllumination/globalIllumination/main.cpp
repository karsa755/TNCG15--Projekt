#include <iostream>
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include "definition.h"
int main(int, char*[])
{
	vertex v = vertex(0.0f, 0.0f, 0.0f, 1.0f);
	direction d = direction(0.0f, 0.0f, 0.0f);
	color c = color(1.0, 1.0, 1.0);

	std::cout << "hello world" << std::endl;
	return 0;
}