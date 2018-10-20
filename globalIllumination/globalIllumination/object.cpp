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

int object::getSurfProperty()
{
	return surfaceProperty;
}

float object::getRadius()
{
	return 0.0f;
}

color object::getColor()
{
	return color();
}

std::vector<triangle> object::getPolyList()
{
	return std::vector<triangle>();
}

object::~object() {
}
