#include "ray.h"



ray::ray(vertex& s, vertex& e, color& c) : 
	start(s), end(e), rayColor(c)
{
}


ray::~ray()
{
}

vertex ray::getStartVec()
{
	return start;
}

vertex ray::getEndVec()
{
	return end;
}
