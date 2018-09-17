#include "camera.h"



camera::camera()
{
	currentEye = &eye1;
}

void camera::switchEye(glm::vec3 & e) {
	currentEye = &e;
}

void camera::render() {
	
}

camera::~camera()
{
}
