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

			//Check intersections
			//std::vector<std::pair<glm::vec3, triangle*>> intersections;
			std::vector<std::pair<glm::vec3, std::pair<object* , triangle* >>> intersections;
			std::pair<glm::vec3, triangle*> tr;
			std::pair<glm::vec3, std::pair<object*, triangle* >> finalBoss;
			for (auto it = begin(objects); it != end(objects); ++it) {
				tr = (*it)->rayIntersect(r);
				finalBoss.first = tr.first;
				finalBoss.second.second = tr.second;
				finalBoss.second.first = *it;
				if (tr.second != nullptr) {
					intersections.push_back(finalBoss);
				}
				else
				{
					if ((*it)->isImplicit() && finalBoss.first != glm::vec3(-1.0f))
					{
						intersections.push_back(finalBoss);

					}
				}
			}

			//we have all intersections, find the closest
			auto it = begin(intersections);
			if (it != end(intersections)) {
				finalBoss = *it;
				++it;
				for (; it != end(intersections); ++it) {
					if (it->first.x < finalBoss.first.x) {
						finalBoss = *it;
					}
				}

				//shoud do stuff with ray...


				//set image color
				if (finalBoss.second.second != nullptr)
				{
					color c = finalBoss.second.second->getSurfaceColor();
					image[i][j].setIntensity(c);
				}
				else
				{
					glm::vec3 normal = (tr.first - finalBoss.second.first->getPosition()) / finalBoss.second.first->getRadius();
					color c = finalBoss.second.first->getColor();
					image[i][j].setIntensity(c);
				}
			}
			else {
				std::cout << "Error, No collision...." << std::endl;
				color c = color(0.0, 0.0, 0.0);
				image[i][j].setIntensity(c);
				++debugCounter;
			}
			

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
