#pragma once

#include "definition.h"
#include "ray.h"
#include <vector>

class pixel
{
public:
	pixel();
	~pixel();

	void setIntensity(color& i);
	color getIntensity();
	void addRay(ray & r);
	std::vector<ray> getRays();
	
private:
	color intensity;
	std::vector<ray> rays;
};

