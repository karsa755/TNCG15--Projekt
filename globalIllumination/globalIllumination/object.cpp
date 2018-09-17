#include "object.h"


object::object(glm::vec3 p) {
	position = p;
}

glm::vec3 object::getPosition() {
	return position;
}

object::~object() {
}
