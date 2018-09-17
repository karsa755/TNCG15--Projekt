#include "polyModel.h"



polyModel::polyModel(std::vector<triangle> t, glm::vec3 p) : object(p)
{
	polyList = t;
}


polyModel::~polyModel()
{
}
