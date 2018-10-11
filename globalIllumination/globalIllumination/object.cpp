#include "object.h"

object::object() {

}

object::object(glm::vec3 p, int prop) {
	position = p;
	surfaceProperty = prop;
}

glm::vec3 object::getPosition() {
	return position;
}

float object::getRadius()
{
	return 0.0f;
}

color object::getColor()
{
	return color();
}

object::~object() {
}
