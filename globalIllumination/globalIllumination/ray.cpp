#include "ray.h"



ray::ray(vertex& s, vertex& e) : 
	start(s), end(e)
{
}

void ray::setTriangle(triangle *t) {
	polygon = t;
}

void ray::setColor(color &c) {
	//Should combine ray color with triangle colors
	rayColor = c;
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
