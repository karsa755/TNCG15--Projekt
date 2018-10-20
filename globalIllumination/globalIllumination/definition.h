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
#define LIGHTWATT 20.0
#define AREA 1.0 //Roof
//#define AREA 4.611 //Tetra
#define eps std::numeric_limits<float>::epsilon()

#define SINGLE_THREAD 100
#define MULTI_THREAD 200


using vertex = glm::vec4;
using direction = glm::vec3;
using color = glm::dvec3;