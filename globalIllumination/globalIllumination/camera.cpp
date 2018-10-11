#include "camera.h"
#include <iostream>


camera::camera(std::vector<object*> ol)
{
	currentEye = &eye2;
	objects = ol;
	findLightSource();
}

void camera::switchEye(glm::vec3 & e) {
	currentEye = &e;
}

triangle * camera::getLightSource()
{
	return lightSource;
}

void camera::findLightSource()
{
	for (auto it = begin(objects); it != end(objects); ++it) 
	{
		if (!(*it)->isImplicit())
		{
			
			std::vector<triangle> polys = (*it)->getPolyList();
			for (auto pIt = begin(polys); pIt != end(polys); ++pIt)
			{
				if (pIt->isEmitting())
				{
					lightSource = &(*pIt);
					return;
				}
			}
		}
	}
}

void camera::render() {

	const float pixelWidth = 2.0 / width;
	const float pixelHeight = 2.0 / height;
	const float deltaPW = pixelWidth / 2.0;
	const float deltaPH = pixelHeight / 2.0;

	int debugCounter = 0;

	for (int i = 0; i < width; i++) {

		std::cout << (i / (float) (width-1.0)) * 100.0 << "% Done..." << std::endl;

		for (int j = 0; j < height; ++j) {
			float y = (1.0 - i * pixelWidth) - deltaPW;
			float z = (j * pixelHeight - 1.0) + deltaPH;
			float x = 0.0;

			vertex s = vertex(currentEye->x, currentEye->y, currentEye->z, 1.0);
			vertex e = vertex(x,y,z,1.0);

			//Create ray between eye and pixelplane
			ray r = ray(s,e);

		}
	}

	std::cout << "Number of failed pixels: " << debugCounter << std::endl;


	//Write
	FILE *f = fopen("out.ppm", "wb");
	fprintf(f, "P6\n%i %i 255\n", width, height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			fputc(image[x][y].getIntensity().x * 255.0, f);   // 0 .. 255
			fputc(image[x][y].getIntensity().y * 255.0, f); // 0 .. 255
			fputc(image[x][y].getIntensity().z * 255.0, f);  // 0 .. 255
		}
	}
	fclose(f);
}

camera::~camera()
{
}
