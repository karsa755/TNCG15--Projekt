#include "ray.h"



ray::ray(vertex& s, vertex& e, triangle& t, color& c) : 
	start(s), end(e), rayColor(c), polygon(t)
{
}


ray::~ray()
{
}
