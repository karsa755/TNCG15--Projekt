#include "camera.h"



camera::camera(std::vector<object*> ol)
{
	currentEye = &eye2;
	objects = ol;
	findLightSource();
	
	distribution = std::uniform_real_distribution<float>(0, 1);
}

void camera::switchEye(glm::vec3 & e) {
	currentEye = &e;
}

triangle  camera::getLightSource()
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
				if (pIt->isEmitter)
				{
					lightSource = (*pIt);
					return;
				}
			}
		}
	}
}


void camera::getLocalCoordSystem(const glm::vec3 &Z, const glm::vec3 &I, glm::vec3 &X, glm::vec3 &Y) {
	glm::vec3 Im = I - (glm::dot(I,Z) * Z);
	X = glm::normalize(Im);
	Y = glm::cross(-X,Z);
}

glm::vec3 camera::localToWorld(const glm::vec3 & X, const glm::vec3 & Y, const glm::vec3 & Z, const glm::vec3 & v)
{
	glm::vec3 out = {X.x*v.x + Y.x*v.y + Z.x*v.z, X.y*v.x + Y.y*v.y + Z.y*v.z, X.z*v.x + Y.z*v.y + Z.z*v.z};
	return out;
}

glm::vec3 camera::worldToLocal(const glm::vec3 & X, const glm::vec3 & Y, const glm::vec3 & Z, const glm::vec3 & v)
{
	glm::mat3 m(X, Y, Z);
	m = glm::inverse(m);
	return m*v;
}


glm::vec3 sampleHemisphere(const float &cosTheta, const float &sidPhi) {
	float sinTheta = sqrtf(1.0 - cosTheta * cosTheta);
	float phi = 2.0 * PI * sidPhi;
	//float x = sinTheta * sinf(phi);
	//float y = -(sinTheta * cosf(phi));
	//return glm::vec3(x,y,cosTheta);

	float z = sinTheta * sinf(phi);
	float x = (sinTheta * cosf(phi));
	return glm::vec3(x, cosTheta, z);
}


std::pair<glm::vec3, std::pair<object*, triangle*>> camera::findClosestIntersection(ray & r)
{

	//find all intersections for a given ray
	std::pair<glm::vec3, triangle*> tr;
	std::pair<glm::vec3, std::pair<object*, triangle*>> intersectionObject;
	std::vector<std::pair<glm::vec3, std::pair<object*, triangle*>>> intersections;
	for (auto it = begin(objects); it != end(objects); ++it) {
		tr = (*it)->rayIntersect(r);
		intersectionObject.first = tr.first;
		intersectionObject.second.second = tr.second;
		intersectionObject.second.first = *it;

		if (tr.second != nullptr) {
			intersections.push_back(intersectionObject);
		}
		else {
			if ((*it)->isImplicit() && intersectionObject.first != glm::vec3(-1.0f)) {
				intersections.push_back(intersectionObject);
			}
		}
	}

	//find closest intersection
	auto it = begin(intersections);
	if (it != end(intersections)) {
		intersectionObject = *it;
		++it;
		float smallestDist = glm::distance((glm::vec3) r.getStartVec(), intersectionObject.first);
		for (; it != end(intersections); ++it) {
			float dist = glm::distance((glm::vec3) r.getStartVec(), it->first);
			if (dist < smallestDist) {
				intersectionObject = *it;
				smallestDist = dist;
			}
		}
		return intersectionObject;
	}
	else {
		std::cout << "Error; No Collision Found..." << std::endl;
		std::pair<glm::vec3, std::pair<object*, triangle*>> toReturn;
		toReturn.second.first = nullptr;
		toReturn.second.second = nullptr;
		return toReturn;
	}
}

color camera::castRay(ray &r, int depth) {

	//find closest intersection
	auto intersection = findClosestIntersection(r);
	vertex midPoint = lightSource.getMidPoint();

	if (intersection.second.first == nullptr) {
		std::cout << "ERROR" << std::endl;
		return color(0.0,0.0,0.0);
	}

	if (intersection.second.second != nullptr && !intersection.second.first->isImplicit() && intersection.second.second->isEmitter) {
		//hitting light source
		//std::cout << "TO LIGHTSOURCE" << std::endl;
		color ret = intersection.second.second->getSurfaceColor();
		return ret;
	}
	
	if (depth > 1) {
		//shadow rays n' stuff
		color dirLight = { 1.0, 1.0, 1.0 };
		glm::vec3 normal = intersection.second.first->isImplicit() ?
			((intersection.first - intersection.second.first->getPosition()) / intersection.second.first->getRadius())
			: intersection.second.second->getNormal();
		
		vertex startPoint = { intersection.first + (normal * 0.001f),1.0f };
		ray toLight(startPoint, midPoint);

		auto closest = findClosestIntersection(toLight);
		if (!closest.second.first->isImplicit() && !closest.second.second->isEmitter) {
			//std::cout << "SHADOW" << std::endl;
			dirLight = { 0.0,0.0,0.0 };
			return dirLight;
		}
		else
		{
			if (closest.second.first->isImplicit())
			{
				return color(0.0, 0.0, 0.0);
			}
		}
		return LIGHTWATT *color(1.0,1.0,1.0) / (PI*AREA);
	}
	else {
		//recursive call
		int N = 2;
		glm::vec3 X;
		glm::vec3 Y;
		glm::vec3 I = intersection.first - (glm::vec3)r.getStartVec();
		glm::vec3 Z = intersection.second.first->isImplicit() ? 
								((intersection.first - intersection.second.first->getPosition()) / intersection.second.first->getRadius()) 
								: intersection.second.second->getNormal();

		getLocalCoordSystem(Z,I,X,Y);

		color finalColor(0.0, 0.0, 0.0);

		color dirLight = { 1.0, 1.0, 1.0 };
		vertex startPoint = { intersection.first + (Z * 0.001f),1.0f };
		ray toLight(startPoint, midPoint);

		auto closest = findClosestIntersection(toLight);
		if (!closest.second.first->isImplicit() && !closest.second.second->isEmitter) {
			//std::cout << "SHADOW" << std::endl;
			dirLight = { 0.0,0.0,0.0 };
			
		}
		else
		{
			if (closest.second.first->isImplicit())
			{
				dirLight = { 0.0,0.0,0.0 };
	
			}
		}
		if (intersection.second.first->getSurfProperty() == DIFFUSE)
		{
			//std::cout << "D" << std::endl;
			double PDF = 1.0f / (2.0*PI);
			for (int n = 0; n < N; ++n) {
				float cosTheta = distribution(generator);
				float sidPhi = distribution(generator);
				glm::vec3 sample = sampleHemisphere(cosTheta, sidPhi);

				glm::vec3 worldSample = localToWorld(X, Y, Z, sample);
				vertex v1 = vertex(intersection.first + worldSample * 0.1f, 1.0f);
				vertex v2 = vertex(worldSample, 1.0f);
				ray outRay(v1, v2);
				outRay.setImportance(r.getImportance() * cosTheta);
				finalColor += (double)cosTheta * castRay(outRay, depth + 1) * PDF;
			}
			finalColor /= (double)N;

			color c;

			if (intersection.second.first->isImplicit()) {
				c = intersection.second.first->getColor();
			}
			else {
				c = intersection.second.second->getSurfaceColor();
			}
			finalColor += LIGHTWATT * dirLight / (PI*AREA);

			return finalColor * c;
		}
		else if (intersection.second.first->getSurfProperty() == MIRROR)
		{
			std::cout << intersection.second.first->getPosition().x << "and " << intersection.second.first->getRadius() << std::endl;
			if (intersection.first.x < 0.01f) {
				std::cout << "Fuck" << std::endl;
				return color(1.0, 0.0, 0.0);
			}
			vertex dir = vertex(intersection.first, 1.0f) - r.getStartVec();
			vertex reflectDir = glm::reflect(dir, vertex(Z, 1.0f));
			vertex startPt = vertex(intersection.first + (glm::vec3)glm::normalize(Z)*0.1f, 1.0f);
			vertex endPt = vertex(intersection.first, 1.0f) + reflectDir;
			ray rMirror(startPt, endPt);
			//std::cout << "x is: " << intersection.first.x << ", y is: " << intersection.first.y <<
			//	", z is: " << intersection.first.z << std::endl;
			return castRay(rMirror, depth);

		}
		else
		{
			// do nothing atm
			return color(0.0, 0.0, 0.0);
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
			r.setImportance(1.0);

			//cast

			auto p = findClosestIntersection(r);

			color c;
			
			/*
			if (p.second.first->isImplicit())
			{
				c = p.second.first->getColor();
			}
			else {
				c = p.second.second->getSurfaceColor();
			}
			*/
			

			c =  castRay(r, 0);
			image[i][j].setIntensity(c);

		}
	}

	std::cout << "Number of failed pixels: " << debugCounter << std::endl;


	//Write
	FILE *f = fopen("out.ppm", "wb");
	fprintf(f, "P6\n%i %i 255\n", width, height);
	for (int y = height; y > 0; y--) {
		for (int x = 0; x < width; x++) {
			fputc(image[x][y].getIntensity().x * 255.0, f);   // 0 .. 255
			fputc(image[x][y].getIntensity().y * 255.0 , f); // 0 .. 255
			fputc(image[x][y].getIntensity().z * 255.0, f);  // 0 .. 255
		}
	}
	fclose(f);
}

camera::~camera()
{
}
