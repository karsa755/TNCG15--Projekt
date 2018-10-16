#include "ray.h"



ray::ray(vertex& s, vertex& e) :
	start(s), end(e), importance(0.0)
{
}

void ray::setTriangle(triangle *t) {
	polygon = t;
}

void ray::setColor(color &c) {
	//Should combine ray color with triangle colors
	rayColor = c;
}

double ray::getImportance()
{
	return importance;
}

void ray::setImportance(double imp)
{
	importance = imp;
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
