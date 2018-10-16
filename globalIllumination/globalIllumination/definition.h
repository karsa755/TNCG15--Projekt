#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/gtc/matrix_transform.hpp>

#define DIFFUSE 0
#define SPECULAR 1
#define ORENNAYAR 2
#define REFRACT 3
#define MIRROR 4
#define PI 3.14159265359
#define LIGHTWATT 1000.0



using vertex = glm::vec4;
using direction = glm::vec3;
using color = glm::dvec3;