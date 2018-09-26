#include "camera.h"
#include <iostream>


camera::camera(std::vector<object*> ol)
{
	currentEye = &eye2;
	objects = ol;
}

void camera::switchEye(glm::vec3 & e) {
	currentEye = &e;
}

void camera::render() {

	const float pixelWidth = 2.0 / width;
	const float pixelHeight = 2.0 / height;
	const float deltaPW = pixelWidth / 2.0;
	const float deltaPH = pixelHeight / 2.0;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; ++j) {
			float y = (1.0 - i * pixelWidth) - deltaPW;
			float z = (j * pixelHeight - 1.0) + deltaPH;
			float x = 0.0;

			vertex s = vertex(currentEye->x, currentEye->y, currentEye->z, 1.0);
			vertex e = vertex(x,y,z,1.0);

			//Create ray between eye and pixelplane
			ray r = ray(s,e);

			//Check intersections
			std::vector<std::pair<glm::vec3, triangle*>> intersections;
			std::pair<glm::vec3, triangle*> tr;
			for (auto it = begin(objects); it != end(objects); ++it) {
				tr = (*it)->rayIntersect(r);
				if (tr.second != nullptr) {
					intersections.push_back(tr);
				}
			}

			//we have all intersections, find the closest
			auto it = begin(intersections);
			tr = *it;
			++it;
			for (; it != end(intersections); ++it) {
				if (it->first.x < tr.first.x) {
					tr = *it;
				}
			}

			//shoud do stuff with ray...


			//set image color
			color c = tr.second->getSurfaceColor();
			image[i][j].setIntensity(c);

		}
	}


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
