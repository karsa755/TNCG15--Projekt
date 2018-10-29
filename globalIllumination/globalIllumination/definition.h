#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/vector_angle.hpp>
#define DIFFUSE 0
#define SPECULAR 1
#define ORENNAYAR 2
#define REFRACT 3
#define MIRROR 4
#define PI 3.14159265359
#define LIGHTWATT 120.0
#define AREA 1.0 //Roof
//#define AREA 4.611 //Tetra
#define eps std::numeric_limits<float>::epsilon()
#define MAX_FLOAT std::numeric_limits<float>::max()

#define SINGLE_THREAD 100
#define MULTI_THREAD 200
#define PHOTONMAPPING 91
#define MONTECARLO 90

struct photon {
	glm::vec3 startPoint;
	glm::vec3 direction;
	float flux;
	photon(glm::vec3 startPt, glm::vec3 dir, float f) : startPoint(startPt), direction(dir), flux(f) {};
	photon() : startPoint(glm::vec3(0.0f)), direction(glm::vec3(0.0f)), flux(0.0f) {};
};

using vertex = glm::vec4;
using direction = glm::vec3;
using color = glm::dvec3;