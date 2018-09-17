#include "pixel.h"



pixel::pixel()
{
	intensity = color(0.0);
}

pixel::~pixel()
{
}

void pixel::setIntensity(color & i)
{
	intensity = i;
}

color pixel::getIntensity() {
	return intensity;
}

void pixel::addRay(ray & r)
{
	rays.push_back(r);
}

std::vector<ray> pixel::getRays()
{
	return rays;
}
