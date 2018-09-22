#include "camera.h"



camera::camera()
{
	currentEye = &eye1;
}

void camera::switchEye(glm::vec3 & e) {
	currentEye = &e;
}

void camera::render() {
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
