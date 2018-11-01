#include "camera.h"



camera::camera(std::vector<object*> ol) : generator(std::chrono::system_clock::now().time_since_epoch().count()),
globalMap(Octree<std::vector<photon>>(boxSize)), causticMap(Octree<std::vector<photon>>(boxSize))
{
	currentEye = &eye2;
	objects = ol;
	findLightSource();
	distribution = std::uniform_real_distribution<float>(0, 1);	
	int renderType = PHOTONMAPPING;
}

triangle  camera::getLightSource()
{
	return light;
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
					light = (*pIt);
					
				}
			}
		}
		else
		{
			if ((*it)->isEmitter())
			{
				photonSource = dynamic_cast<lightSource*>(*it);
			}
		}
	}
}


void camera::getLocalCoordSystem(const glm::vec3 &Z, const glm::vec3 &I, glm::vec3 &X, glm::vec3 &Y) {
	glm::vec3 Im = I - (glm::dot(I,Z) * Z);
	X = glm::normalize(Im);
	Y = glm::normalize(glm::cross(-X,Z));


}

glm::vec3 camera::localToWorld(const glm::vec3 & X, const glm::vec3 & Y, const glm::vec3 & Z, const glm::vec3 & v, const glm::vec3 &t)
{

	glm::mat4 Mrot(1.0f);
	glm::mat4 Mtrans(1.0f);
	Mrot[0][0] = X.x;
	Mrot[1][0] = X.y;
	Mrot[2][0] = X.z;
	Mrot[3][0] = 0.0f;
	Mrot[0][1] = Y.x;
	Mrot[1][1] = Y.y;
	Mrot[2][1] = Y.z;
	Mrot[3][1] = 0.0f;
	Mrot[0][2] = Z.x;
	Mrot[1][2] = Z.y;
	Mrot[2][2] = Z.z;
	Mrot[3][2] = 0.0f;
	Mrot[0][3] = 0.0f;
	Mrot[1][3] = 0.0f;
	Mrot[2][3] = 0.0f;
	Mrot[3][3] = 1.0f;

	Mtrans[0][3] = -t.x;
	Mtrans[1][3] = -t.y;
	Mtrans[2][3] = -t.z;
	glm::mat4 M = Mrot * Mtrans;
	glm::mat4 Minv = glm::inverse(M);
	return (glm::vec3)(Minv * glm::vec4(v, 1.0f));
}

glm::vec3 camera::worldToLocal(const glm::vec3 & X, const glm::vec3 & Y, const glm::vec3 & Z, const glm::vec3 & v)
{
	glm::mat3 m(X, Y, Z);
	m = glm::inverse(m);
	return m*v;
}

void camera::setBranchFactor(int f)
{
	FACTOR = f;
}

void camera::setShadowRays(int n)
{
	SHADOWRAYS = n;
}

void camera::setDepth(int d)
{
	MAXDEPTH = d;
}


int camera::getWidth()
{
	return width;
}

int camera::getHeight()
{
	return height;
}

const glm::vec3 * camera::getCurrentEye()
{
	return currentEye;
}

void camera::setPixelValue(color c, int i, int j)
{
	image[i][j].setIntensity(c);
}

void camera::setRenderingMode(int mode)
{
	MODE = mode;
}

void camera::clearConsole()
{
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);

}


void camera::setRenderType(int type)
{
	renderType = type;
}

void camera::printContext()
{
	std::string s;
	s = (MODE == SINGLE_THREAD) ? " [Single Thread]" : " [" + std::to_string(std::thread::hardware_concurrency()) + " Threads]";
	std::cout << "Rendering " << width << "x" << height << s << std::endl;
	std::cout << "Branching Factor: " << FACTOR << std::endl;
	std::cout << "Number Shadow Rays: " << SHADOWRAYS << std::endl;
	std::cout << "Max Depth: " << MAXDEPTH << std::endl;
	std::cout << "Init ray: " << initRAY << std::endl;
	if (renderType == PHOTONMAPPING) {
		std::cout <<"Globab Photons: " << storedGlobal << " (" << globalNr << " emitted)" << std::endl;
		std::cout << "Caustic Photons: " << storedCaustic << " (" << causticNr << " emitted)" << std::endl << std::endl;
	}
}


glm::vec3 sampleHemisphere(const float &cosTheta, const float &sidPhi) {
	float sinTheta = sqrtf(1.0 - cosTheta * cosTheta);
	float phi = 2.0 * PI * sidPhi;
	float x = sinTheta * cosf(phi);
	float y = sinTheta * sinf(phi);
	return glm::vec3(x,y, cosTheta);
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
			if ( !(*it)->isEmitter() && (*it)->isImplicit() && intersectionObject.first.x < MAX_FLOAT / 2.0f
				&& intersectionObject.first.y < MAX_FLOAT / 2.0f && intersectionObject.first.z < MAX_FLOAT / 2.0f) {
				intersections.push_back(intersectionObject);
			}
			else
			{
				if ((*it)->isEmitter())
				{
					intersections.push_back(intersectionObject);
				}
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
	std::vector<glm::vec3> lightSamples;
	int N = FACTOR;
	double rho;
	if (intersection.second.first->isImplicit())
	{
		rho = intersection.second.first->getRho() / PI;
	}
	else
	{
		rho = intersection.second.second->getRho() / PI;
	}

	for (int i = 0; i < SHADOWRAYS; ++i)
	{
		float u = distribution(generator);
		float v = (1 - u) * distribution(generator);
		lightSamples.emplace_back(light.sampleTriangle(u, v));
	}

	if (intersection.second.first == nullptr) {
		std::cout << "ERROR" << std::endl;
		return color(0.0,0.0,0.0);
	}

	color c;
	if (intersection.second.first->isImplicit()) {
		c = intersection.second.first->getColor();
	}
	else {
		c = intersection.second.second->getSurfaceColor();
	}

	//hitting light source
	if (intersection.second.second != nullptr && !intersection.second.first->isImplicit() && intersection.second.second->isEmitter) {
		color ret = intersection.second.second->getSurfaceColor();
		if (depth == 0) return color(1.0, 1.0, 1.0) * LIGHTWATT / (2.0 * PI * AREA); // if first bounce		
		
		return ret*LIGHTWATT / (2.0 * PI * AREA);
	}

	float russianRoulette = distribution(generator);
	glm::vec3 normal = intersection.second.first->isImplicit() ?
		((intersection.first - intersection.second.first->getPosition()) / intersection.second.first->getRadius())
		: intersection.second.second->getNormal();

	vertex startPoint = { intersection.first + (normal * 0.01f),1.0f };
	double lightHits = 0.0;
	for (int i = 0; i < SHADOWRAYS; ++i)
	{
		vertex n = vertex(lightSamples.at(i), 1.0f);
		ray toLight(startPoint, n);

		auto closest = findClosestIntersection(toLight);
		if (!closest.second.first->isImplicit() && closest.second.second->isEmitter) {
			lightHits += (std::max(0.0f, glm::dot(glm::vec3(0.0f, 0.0f, -1.0f), glm::normalize((glm::vec3)startPoint - lightSamples.at(i))))
				* std::max(0.0f, glm::dot(normal, glm::normalize(lightSamples.at(i) - (glm::vec3)startPoint))))
				/ std::pow(std::max(1.0, (double)glm::distance(intersection.first, closest.first)), 2.0);
		}
	}
	

	//if(depth >= MAXDEPTH) {
	if (depth > 0 &&  (depth >= MAXDEPTH || russianRoulette >= 0.2f) ) { //max depth 20

		return (((rho)*(double)LIGHTWATT *color(lightHits, lightHits, lightHits))  / ( 2.0 * (double)PI * (double)AREA * (double)SHADOWRAYS));
	}
	else {
		//recursive call
		glm::vec3 X(0.0f);
		glm::vec3 Y(0.0f);
		glm::vec3 I = glm::normalize((glm::vec3)r.getStartVec() - (glm::vec3)r.getEndVec());
		glm::vec3 Z = intersection.second.first->isImplicit() ? 
								((intersection.first - intersection.second.first->getPosition()) / intersection.second.first->getRadius()) 
								: intersection.second.second->getNormal();

		getLocalCoordSystem(Z,I,X,Y);

		color finalColor(0.0, 0.0, 0.0);		
		if (intersection.second.first->getSurfProperty() == DIFFUSE)
		{
			double PDF = 1.0 / (2.0 * PI);
			
			for (int n = 0; n < N; ++n) {
				float cosTheta = distribution(generator);
				float sidPhi = distribution(generator);
				glm::vec3 sample = sampleHemisphere(cosTheta, sidPhi);
				
				glm::vec3 worldSample = localToWorld(X, Y, Z, sample, intersection.first);

				glm::vec3 directionWorld = glm::normalize(worldSample - intersection.first);

				vertex v1 = vertex(intersection.first + directionWorld * 0.00001f, 1.0f);
				vertex v2 = vertex(worldSample, 1.0f);
				ray outRay(v1, v2);
				finalColor += (double)cosTheta * castRay(outRay, depth + 1);
			}
			finalColor /= ((double)N * PDF);
			color dirLight = { lightHits, lightHits, lightHits };

			color finC = rho * ( ( ((double)LIGHTWATT * dirLight) / ((double)SHADOWRAYS*(double)AREA*2.0*(double)PI)) + (finalColor) ) * c;
			return finC;
		}
		else if (intersection.second.first->getSurfProperty() == MIRROR)
		{

			ray rMirror = mirror(intersection, r, normal);
			return castRay(rMirror, depth);
		}
		else if(intersection.second.first->getSurfProperty() == ORENNAYAR)
		{
			float standardDev = 0.3f * 0.3f;
			
			float A = 1.0f - (standardDev / (2.0f*(standardDev + 0.33f)));
			float B = (0.45f * standardDev) / (standardDev + 0.09f);
			glm::vec3 dirIn = ((glm::vec3)r.getStartVec() - intersection.first);
			glm::vec3 projectedIn = glm::normalize(dirIn - (glm::dot(dirIn, normal) * normal));
			float Fr = 0.0f;
			
			float thetaIN = glm::dot(normal, dirIn);
			thetaIN = acosf(thetaIN);
			color indir(0.0, 0.0, 0.0);
			float PDF = 1 / (2.0 * PI);
			for (int n = 0; n < N; ++n) {
				float cosTheta = distribution(generator);
				float sidPhi = distribution(generator);
				float thetaOUT = acosf(cosTheta);
				float alpha = std::max(thetaIN, thetaOUT);
				float beta = std::min(thetaIN, thetaOUT);

				glm::vec3 sample = sampleHemisphere(cosTheta, sidPhi);
				glm::vec3 worldSample = localToWorld(X, Y, Z, sample, intersection.first);
				glm::vec3 directionWorld = (worldSample - intersection.first);
				glm::vec3 projectedOut = glm::normalize(directionWorld - (glm::dot(directionWorld, normal) * normal));
				float deltaPhi = glm::dot(projectedIn, projectedOut);
				Fr = rho *  (A + B * std::max(0.0f, deltaPhi * sinf(alpha) * sinf(beta)));
				vertex v1 = vertex(intersection.first + directionWorld * 0.00001f, 1.0f);
				vertex v2 = vertex(worldSample, 1.0f);
				ray outRay(v1, v2);
				indir += castRay(outRay, depth + 1) * (double)Fr * (double)cosTheta;
			}
			indir /= ((double)N*PDF);
			color dirLight = { lightHits, lightHits, lightHits };
			color finalC = c*(((Fr*(double)LIGHTWATT * dirLight) / ((double)SHADOWRAYS*(double)AREA*2.0*(double)PI)) + indir);
			return finalC;
		}
		else if(intersection.second.first->getSurfProperty() == REFRACT)
		{

			vertex v(0.0f);
			ray rayReflect(v, v);
			ray rayRefract(v, v);
			float R, T;
			bool refraction = refract(intersection, r, normal, rayReflect, rayRefract, R, T);
			if (!refraction)
			{
				return castRay(rayReflect, depth);
			}
			else
			{
				if (R < 0.05f) {
					float T = 1.0;
					color cRefract = (double)T * castRay(rayRefract, depth);
					return  cRefract;
				}
				else {
					color cReflect = (double)R * castRay(rayReflect, depth);
					color cRefract = (double)T * castRay(rayRefract, depth);
					return  cRefract + cReflect;
				}
			}
		}
		else
		{
			return color(0.0, 0.0, 0.0); //do nothing
		}
	}
}

color camera::photonMapRender(ray & r)
{
	//find closest intersection
	auto intersection = findClosestIntersection(r);
	int N = FACTOR;
	double rho;
	if (intersection.second.first->isImplicit())
	{
		rho = intersection.second.first->getRho() / PI;
	}
	else
	{
		rho = intersection.second.second->getRho() / PI;
	}

	if (intersection.second.first == nullptr) {
		std::cout << "ERROR" << std::endl;
		return color(0.0, 0.0, 0.0);
	}

	color c;
	if (intersection.second.first->isImplicit()) {
		c = intersection.second.first->getColor();
	}
	else {
		c = intersection.second.second->getSurfaceColor();
	}
	if (intersection.second.first->isEmitter())
	{

		return c * LIGHTWATT / (2.0 * PI * dynamic_cast<lightSource*>(intersection.second.first)->getArea());
	}
	std::vector<glm::vec3> lightSamples;

	for (int i = 0; i < SHADOWRAYS; ++i)
	{
		float u = distribution(generator);
		float v = distribution(generator);
		lightSamples.emplace_back(glm::vec3(photonSource->sampleCircle(u, v), photonSource->getPosition().z));
	}



	glm::vec3 normal = intersection.second.first->isImplicit() ?
		((intersection.first - intersection.second.first->getPosition()) / intersection.second.first->getRadius())
		: intersection.second.second->getNormal();

	vertex startPoint = { intersection.first + (normal * 0.01f),1.0f };
	double lightHits = 0.0;
	for (int i = 0; i < SHADOWRAYS; ++i)
	{
		vertex n = vertex(lightSamples.at(i), 1.0f);
		ray toLight(startPoint, n);

		auto closest = findClosestIntersection(toLight);
		if (closest.second.first->isEmitter()) {
			lightHits += (std::max(0.0f, glm::dot(glm::vec3(0.0f, 0.0f, -1.0f), glm::normalize((glm::vec3)startPoint - lightSamples.at(i))))
				* std::max(0.0f, glm::dot(normal, glm::normalize(lightSamples.at(i) - (glm::vec3)startPoint))))
				/ std::pow(std::max(1.0, (double)glm::distance(intersection.first, closest.first)), 2.0);
		}
	}
	
	color directLight = (rho * (double)LIGHTWATT * color(lightHits, lightHits, lightHits)) / ((double)SHADOWRAYS*2.0*(double)PI);
	if (intersection.second.first->getSurfProperty() == DIFFUSE || intersection.second.first->getSurfProperty() == ORENNAYAR)
	{
		float r0Caustic = 0.1f*0.1f;
		float r0Global = 0.4f*0.4f;
		float radiance = 0.0f;

		int Gxm, Gxe, Gym, Gye, Gzm, Gze;
		int Cxm, Cxe, Cym, Cye, Czm, Cze;

		Gxm = static_cast<int>(round(intersection.first.x - r0Global)) + offsetVec.x;
		Gxe = static_cast<int>(round(intersection.first.x + r0Global)) + offsetVec.x;
		Gym = static_cast<int>(round(intersection.first.y - r0Global)) + offsetVec.y;
		Gye = static_cast<int>(round(intersection.first.y + r0Global)) + offsetVec.y;
		Gzm = static_cast<int>(round(intersection.first.z - r0Global)) + offsetVec.z;
		Gze = static_cast<int>(round(intersection.first.z + r0Global)) + offsetVec.z;
		Cxm = static_cast<int>(round(intersection.first.x - r0Caustic)) + offsetVec.x;
		Cxe = static_cast<int>(round(intersection.first.x + r0Caustic)) + offsetVec.x;
		Cym = static_cast<int>(round(intersection.first.y - r0Caustic)) + offsetVec.y;
		Cye = static_cast<int>(round(intersection.first.y + r0Caustic)) + offsetVec.y;
		Czm = static_cast<int>(round(intersection.first.z - r0Caustic)) + offsetVec.z;
		Cze = static_cast<int>(round(intersection.first.z + r0Caustic)) + offsetVec.z;

		
		for (int i = Gxm; i <= Gxe; ++i) {
			for (int j = Gym; j <= Gye; ++j) {
				for (int k = Gzm; k <= Gze; ++k) {
					std::vector<photon> &currentPhotonsGlobal = globalMap(i, j, k);
					for (photon p : currentPhotonsGlobal)
					{
						float dist = glm::distance(intersection.first, p.startPoint);
						if (dist < r0Global)
							radiance += (rho * p.flux) / ((float)PI * dist);
					}
				}
			}
		}

		for (int i = Cxm; i <= Cxe; ++i) {
			for (int j = Cym; j <= Cye; ++j) {
				for (int k = Czm; k <= Cze; ++k) {
					std::vector<photon> &currentPhotonsCaustic = causticMap(i, j, k);
					for (photon p : currentPhotonsCaustic)
					{
						float dist = glm::distance(intersection.first, p.startPoint);
						if (dist < r0Caustic)
							radiance += (rho * p.flux) / ((float)PI * dist);
					}
				}
			}
		}
		
		//color indirLight = calcIndirectLight(r, 0);
		return c * ((double)radiance + directLight);
	}
	else if (intersection.second.first->getSurfProperty() == REFRACT)
	{
		vertex v(0.0f);
		ray rayReflect(v, v);
		ray rayRefract(v, v);
		float R, T;
		bool refraction = refract(intersection, r, normal, rayReflect, rayRefract, R, T);
		if (!refraction)
		{
			return photonMapRender(rayReflect);
		}
		else
		{
			if (R < 0.05f) {
				float T = 1.0;
				color cRefract = (double)T * photonMapRender(rayRefract);
				return  cRefract;
			}
			else {
				color cReflect = (double)R * photonMapRender(rayReflect);
				color cRefract = (double)T * photonMapRender(rayRefract);
				return  cRefract + cReflect;
			}
		}
	}
	else if (intersection.second.first->getSurfProperty() == MIRROR)
	{
		ray rMirror = mirror(intersection, r, normal);
		return photonMapRender(rMirror);
	}
	else
	{
		return color(0.0, 0.0, 0.0); //wtf
	}
}

color camera::calcIndirectLight(ray & r, int depth)
{
	//find closest intersection
	auto intersection = findClosestIntersection(r);
	int N = FACTOR;
	double rho;
	if (intersection.second.first->isImplicit())
	{
		rho = intersection.second.first->getRho() / PI;
	}
	else
	{
		rho = intersection.second.second->getRho() / PI;
	}

	if (intersection.second.first == nullptr) {
		std::cout << "ERROR" << std::endl;
		return color(0.0, 0.0, 0.0);
	}

	color c;
	if (intersection.second.first->isImplicit()) {
		c = intersection.second.first->getColor();
	}
	else {
		c = intersection.second.second->getSurfaceColor();
	}

	if (intersection.second.first->isEmitter()) {
		return c * LIGHTWATT / (2.0 * PI * dynamic_cast<lightSource*>(intersection.second.first)->getArea());
	}

	float russianRoulette = distribution(generator);
	glm::vec3 normal = intersection.second.first->isImplicit() ?
		((intersection.first - intersection.second.first->getPosition()) / intersection.second.first->getRadius())
		: intersection.second.second->getNormal();

	
	if (depth > 0 && (depth >= MAXDEPTH || russianRoulette >= 0.2f)) { //max depth 20
		float r0Global = 0.5f*0.5f;
		int x = static_cast<int>(round(intersection.first.x)) + offsetVec.x;
		int y = static_cast<int>(round(intersection.first.y)) + offsetVec.y;
		int z = static_cast<int>(round(intersection.first.z)) + offsetVec.z;
		float radiance = 0.0f;
		for (int i = -1; i < 2; ++i)
		{
			for (int j = -1; j < 2; ++j)
			{
				for (int k = -1; k < 2; ++k)
				{
					std::vector<photon> &currentPhotonsGlobal = globalMap(x + i, y + j, z + k);
					for (photon p : currentPhotonsGlobal)
					{
						float dist = glm::distance(intersection.first, p.startPoint);
						if (dist < r0Global)
						{
							radiance += (rho * p.flux) / ((float)PI * dist);
						}
					}
				}
			}
		}
		return (color(1.0, 1.0, 1.0)) / (2.0*PI);
	}
	else {
		//recursive call
		glm::vec3 X(0.0f);
		glm::vec3 Y(0.0f);
		glm::vec3 I = glm::normalize((glm::vec3)r.getStartVec() - (glm::vec3)r.getEndVec());
		glm::vec3 Z = intersection.second.first->isImplicit() ?
			((intersection.first - intersection.second.first->getPosition()) / intersection.second.first->getRadius())
			: intersection.second.second->getNormal();

		getLocalCoordSystem(Z, I, X, Y);

		color finalColor(0.0, 0.0, 0.0);
		if (intersection.second.first->getSurfProperty() == DIFFUSE)
		{
			double PDF = 1.0 / (2.0 * PI);

			for (int n = 0; n < N; ++n) {
				float cosTheta = distribution(generator);
				float sidPhi = distribution(generator);
				glm::vec3 sample = sampleHemisphere(cosTheta, sidPhi);

				glm::vec3 worldSample = localToWorld(X, Y, Z, sample, intersection.first);

				glm::vec3 directionWorld = glm::normalize(worldSample - intersection.first);

				vertex v1 = vertex(intersection.first + directionWorld * 0.00001f, 1.0f);
				vertex v2 = vertex(worldSample, 1.0f);
				ray outRay(v1, v2);
				finalColor += (double)cosTheta * calcIndirectLight(outRay, depth + 1) * rho;
			}
			finalColor /= ((double)N * PDF);
			return c * finalColor;
		}
		else if (intersection.second.first->getSurfProperty() == MIRROR)
		{
			ray rMirror = mirror(intersection, r, normal);
			return calcIndirectLight(rMirror, depth);
		}
		else if (intersection.second.first->getSurfProperty() == ORENNAYAR)
		{
			float standardDev = 0.3f * 0.3f;

			float A = 1.0f - (standardDev / (2.0f*(standardDev + 0.33f)));
			float B = (0.45f * standardDev) / (standardDev + 0.09f);
			glm::vec3 dirIn = ((glm::vec3)r.getStartVec() - intersection.first);
			glm::vec3 projectedIn = glm::normalize(dirIn - (glm::dot(dirIn, normal) * normal));
			float Fr = 0.0f;

			float thetaIN = glm::dot(normal, dirIn);
			thetaIN = acosf(thetaIN);
			color indir(0.0, 0.0, 0.0);
			float PDF = 1 / (2.0 * PI);
			for (int n = 0; n < N; ++n) {
				float cosTheta = distribution(generator);
				float sidPhi = distribution(generator);
				float thetaOUT = acosf(cosTheta);
				float alpha = std::max(thetaIN, thetaOUT);
				float beta = std::min(thetaIN, thetaOUT);

				glm::vec3 sample = sampleHemisphere(cosTheta, sidPhi);
				glm::vec3 worldSample = localToWorld(X, Y, Z, sample, intersection.first);
				glm::vec3 directionWorld = (worldSample - intersection.first);
				glm::vec3 projectedOut = glm::normalize(directionWorld - (glm::dot(directionWorld, normal) * normal));
				float deltaPhi = glm::dot(projectedIn, projectedOut);
				Fr = rho * (A + B * std::max(0.0f, deltaPhi * sinf(alpha) * sinf(beta)));
				vertex v1 = vertex(intersection.first + directionWorld * 0.00001f, 1.0f);
				vertex v2 = vertex(worldSample, 1.0f);
				ray outRay(v1, v2);
				indir += calcIndirectLight(outRay, depth + 1) * (double)Fr * (double)cosTheta;
			}
			indir /= ((double)N*PDF);
			
			color finalC = c * indir;
			return finalC;
		}
		else if (intersection.second.first->getSurfProperty() == REFRACT)
		{

			vertex v(0.0f);
			ray rayReflect(v, v);
			ray rayRefract(v, v);
			float R, T;
			bool refraction = refract(intersection, r, normal, rayReflect, rayRefract, R, T);
			if (!refraction)
			{
				return calcIndirectLight(rayReflect, depth);
			}
			else
			{
				if (R < 0.05f) {
					float T = 1.0;
					color cRefract = (double)T * calcIndirectLight(rayRefract, depth);
					return  cRefract;
				}
				else {
					color cReflect = (double)R * calcIndirectLight(rayReflect, depth);
					color cRefract = (double)T * calcIndirectLight(rayRefract, depth);
					return  cRefract + cReflect;
				}
			}
		}
		else
		{
			return color(0.0, 0.0, 0.0); //do nothing
		}
	}
}

void camera::setInitRay(int ray)
{
	initRAY = ray;
}

int camera::getInitRay()
{
	return initRAY;
}

void old_multi(camera *c, int line, int thr) {
	const float pixelWidth = 2.0 / c->getWidth();
	const float pixelHeight = 2.0 / c->getHeight();
	const float deltaPW = pixelWidth / 2.0;
	const float deltaPH = pixelHeight / 2.0;
	int renderAs = c->getRenderType();

	int width = c->getWidth();
	int initNum = c->getInitRay();
	for (int i = 0; i < width; ++i) {
		color col(0.0);
		for (int num = 0; num < initNum; ++num) {
			float newDeltaPW = pixelWidth * c->distribution(c->generator);
			float newDeltaPH = pixelHeight * c->distribution(c->generator);
			float y = (1.0 - i * pixelWidth) - newDeltaPW;
			float z = (line * pixelHeight - 1.0) + newDeltaPH;
			float x = 0.0;

			vertex s = vertex(c->getCurrentEye()->x, c->getCurrentEye()->y, c->getCurrentEye()->z, 1.0);
			vertex e = vertex(x, y, z, 1.0);

			ray r = ray(s, e);
			col += (renderAs == PHOTONMAPPING) ? c->photonMapRender(r) : c->castRay(r, 0);
		}
		col /= (double)initNum;
		c->setPixelValue(col,i,line);
	}
}

double camera::sigMoidNormalize(double pixVal, double alpha, double beta)
{
	double res = 1.0 / ( 1.0 + std::exp(-((pixVal - beta) / alpha)) );
	return res;
}

void multi(camera *c, int thr) {
	int currentLine = c->getAndUpdateCurrentLine();
	int MAX = c->getHeight();
	int cores = std::thread::hardware_concurrency();

	while (currentLine < MAX) {
		const float pixelWidth = 2.0 / c->getWidth();
		const float pixelHeight = 2.0 / c->getHeight();
		const float deltaPW = pixelWidth / 2.0;
		const float deltaPH = pixelHeight / 2.0;
		int renderAs = c->getRenderType();

		int width = c->getWidth();
		int initNum = c->getInitRay();
		for (int i = 0; i < width; ++i) {
			color col(0.0);
			for (int num = 0; num < initNum; ++num) {
				float newDeltaPW = pixelWidth * c->distribution(c->generator);
				float newDeltaPH = pixelHeight * c->distribution(c->generator);
				float y = (1.0 - i * pixelWidth) - newDeltaPW;
				float z = (currentLine * pixelHeight - 1.0) + newDeltaPH;
				float x = 0.0;

				vertex s = vertex(c->getCurrentEye()->x, c->getCurrentEye()->y, c->getCurrentEye()->z, 1.0);
				vertex e = vertex(x, y, z, 1.0);

				ray r = ray(s, e);
				col += (renderAs == PHOTONMAPPING) ? c->photonMapRender(r) : c->castRay(r, 0);
			}
			col /= (double)initNum;
			c->setPixelValue(col, i, currentLine);
		}

		//get next line
		currentLine = c->getAndUpdateCurrentLine();

		//printing
		if (currentLine % cores == 0 || currentLine == MAX) {
			c->clearConsole();
			c->printContext();
			std::cout << "Progress: " << (currentLine) << " of " << MAX << " rows complete" << std::endl;
		}
	}
}

void camera::render() {

	const float pixelWidth = 2.0 / width;
	const float pixelHeight = 2.0 / height;
	const float deltaPW = pixelWidth / 2.0;
	const float deltaPH = pixelHeight / 2.0;

	int debugCounter = 0;

	if (MODE == SINGLE_THREAD) {
		
		if (getRenderType() == PHOTONMAPPING)
		{
			generateGlobalPhotonMap();
			generateCausticPhotonMap();
		}

		int initNum = getInitRay();
		for (int i = 0; i < width; i++) {		
			for (int j = 0; j < height; ++j) {
				color c(0.0);
				for (int n = 0; n < initNum; ++n) {
					float newDeltaPW = pixelWidth * distribution(generator);
					float newDeltaPH = pixelHeight * distribution(generator);
					float y = (1.0 - i * pixelWidth) - newDeltaPW;
					float z = (j * pixelHeight - 1.0) + newDeltaPH;
					float x = 0.0;

					vertex s = vertex(currentEye->x, currentEye->y, currentEye->z, 1.0);
					vertex e = vertex(x, y, z, 1.0);

					//Create ray between eye and pixelplane
					ray r = ray(s, e);
					c += (getRenderType() == PHOTONMAPPING) ? photonMapRender(r) : castRay(r, 0);
				}
				c /= (double)initNum;
				setPixelValue(c, i, j);
			}
			
			clearConsole();
			printContext();
			std::cout << "Progress: " << (i + 1) << " of " << width << " rows complete" << std::endl;
		 }
	}
	else if (MODE == MULTI_THREAD) {	
		std::cout << "Started" << std::endl;

		unsigned cores = std::thread::hardware_concurrency();
		if (getRenderType() == PHOTONMAPPING)
		{
			generateGlobalPhotonMap();
			generateCausticPhotonMap();
		}

		std::vector<std::thread> pool;
		int line = 0;
		for (unsigned i = 0; i < cores; ++i) {
			pool.emplace_back(std::thread{ multi, this, i});
		}
		for (auto& t : pool) { t.join(); }

		/*
		while (line != height) {
			for (unsigned i = 0; i < cores; ++i) {
				if (line < height) {
					pool.emplace_back(std::thread{ old_multi, this, line, i });
					++line;
				}
			}
			for (auto& t : pool) {t.join();}
			pool.clear();

			//printing
			clearConsole();
			printContext();
			std::cout << "Progress: " << (line) << " of " << height << " rows complete" << std::endl;
		}	
		*/

	}
	std::cout << "Number of failed pixels: " << debugCounter << std::endl;
	
	//Write
	FILE *f = fopen("out.ppm", "wb");
	fprintf(f, "P6\n%i %i 255\n", width, height);
	for (int y = height; y > 0; y--) {
		for (int x = 0; x < width; x++) {
			fputc(std::pow(std::min((image[x][y].getIntensity().x), 1.0), 0.5) * 255, f);   // 0 .. 255
			fputc(std::pow(std::min((image[x][y].getIntensity().y), 1.0), 0.5) * 255, f); // 0 .. 255
			fputc(std::pow(std::min((image[x][y].getIntensity().z), 1.0), 0.5) * 255, f);  // 0 .. 255
		}
	}
	fclose(f);
}

int camera::getRenderType()
{
	return renderType;
}

camera::~camera()
{

}

ray camera::mirror(const std::pair<glm::vec3, std::pair<object*, triangle*>>& intersection,  ray r, glm::vec3 normal)
{
	glm::vec3 dir3 = (intersection.first - (glm::vec3)r.getStartVec());
	glm::vec3 reflectDir = glm::reflect(dir3, normal);
	vertex startPt = vertex(intersection.first + normal * 0.1f, 1.0f);
	vertex endPt = vertex(intersection.first + reflectDir, 1.0f);
	ray rMirror(startPt, endPt);
	return rMirror;
}

bool camera::refract(const std::pair<glm::vec3, std::pair<object*, triangle*>>& intersection, ray r,
	glm::vec3 normal, ray & reflect, ray & refract, float & Rnew, float &Tnew)
{
	float n2 = 1.5f;
	float n1 = 1.0f;
	float ratio = 0.0f;
	glm::vec3 oppositeDir = glm::normalize((glm::vec3)r.getStartVec() - intersection.first);
	glm::vec3 dirIn = glm::normalize(intersection.first - (glm::vec3)r.getStartVec());

	float thetaIN = glm::angle(oppositeDir, normal);
	if (std::fabs(thetaIN) >= (float)PI / 2.0f) //inside sphere
	{
		std::swap(n1, n2);
		normal *= -1.0f;
		thetaIN = glm::angle(oppositeDir, normal);
	}

	float R0 = std::powf(((n1 - n2) / (n1 + n2)), 2.0f);
	float R = R0 + (1 - R0)* std::powf(1 - cosf(thetaIN), 5.0f);
	float T = 1 - R;

	if (n1 > n2)
	{
		float brewsterAngle = asinf(n2 / n1);
		thetaIN = glm::angle(oppositeDir, normal);
		if (thetaIN > brewsterAngle) //total reflection only
		{
			glm::vec3 reflectDir = glm::reflect(dirIn, normal);
			vertex startPt = vertex(intersection.first + (glm::vec3)reflectDir * 0.1f, 1.0f);
			vertex endPt = vertex(intersection.first + reflectDir, 1.0f);
			ray rMirror(startPt, endPt);
			reflect = rMirror;
			Rnew = 1;
			return false;
		}

	}
	ratio = n1 / n2;

	glm::vec3 reflectDir = glm::reflect(dirIn, normal);
	glm::vec3 refractDir = glm::refract(dirIn, normal, ratio); //hmm?

	vertex startPtReflect = vertex(intersection.first + reflectDir * 0.1f, 1.0f);
	vertex startPtRefract = vertex(intersection.first + refractDir * 0.01f, 1.0f);
	
	//for reflection
	vertex endPtReflect = vertex(intersection.first, 1.0f) + vertex(reflectDir, 1.0f);
	vertex endPtRefract = vertex(intersection.first, 1.0f) + vertex(refractDir, 1.0f);

	ray rayReflect(startPtReflect, endPtReflect);
	ray rayRefract(startPtRefract, endPtRefract);
	reflect = rayReflect;
	refract = rayRefract;
	Rnew = R;
	Tnew = T;
	return true;
}

void multiGenerate(camera *c , int d, int t, int thr) {
	unsigned cores = (c->MODE == SINGLE_THREAD) ? 1 : std::thread::hardware_concurrency();
	for (int i = 0; i < d; ++i) {
		float u = c->distribution(c->generator);
		float v = c->distribution(c->generator);
		glm::vec3 lightPos = glm::vec3(c->photonSource->sampleCircle(u, v), c->photonSource->getPosition().z);
		glm::vec3 lightNormal = c->photonSource->getNormal();

		//sample hemisphere
		float uHemi = c->distribution(c->generator);
		float vHemi = c->distribution(c->generator);
		glm::vec3 localPoint = sampleHemisphere(uHemi, vHemi);
		glm::vec3 outDir = glm::normalize(-localPoint);	//Assumes lightsource in 
		glm::vec3 worldPoint = lightPos + outDir; // Get world point
		
												  //create ray
		vertex startPoint = vertex(lightPos, 1.0f);
		vertex endPoint = vertex(worldPoint, 1.0f);
		ray r(startPoint, endPoint);

		bool multicore = false;
		auto intersection = c->findClosestIntersection(r);
		if (t == GLOBAL || (t == CAUSTIC && (intersection.second.first->getSurfProperty() == MIRROR || intersection.second.first->getSurfProperty() == REFRACT)))
		{
			c->bouncePhoton(r, 0, t);
		}
		
		if (t == GLOBAL && (i+1) % (int)floor(d / (10)) == 0 && (c->threadPrinter == thr)) {
			c->clearConsole();
			std::cout << "(GLOBAL) Emitting " << c->globalNr << " Photons" << std::endl;
			for (int q = 0; q < cores; ++q) {
				std::cout << "Thread (" << q << "): " << (i + 1) << " of " << d << " done" << std::endl;
			}
			c->threadPrinter = (c->threadPrinter >= (cores-1)) ? 0 : c->threadPrinter+1;
		}
		if (t == CAUSTIC && (i + 1) % (int)floor(d / (10)) == 0 && (c->threadPrinter == thr)) {
			c->clearConsole();
			std::cout << "(CAUSTIC) Emitting " << c->causticNr << " Photons" << std::endl;
			for (int q = 0; q < cores; ++q) {
				std::cout << "Thread (" << q << "): " << (i + 1) << " of " << d << " done" << std::endl;
			}
			c->threadPrinter = (c->threadPrinter >= (cores-1)) ? 0 : c->threadPrinter + 1;
		}
	}
}


void camera::generateGlobalPhotonMap()
{
	unsigned cores = (MODE == SINGLE_THREAD) ? 1 : std::thread::hardware_concurrency();
	int delta = (int)floor(globalNr / cores);
	
	std::vector<std::thread> pool;

	for (unsigned i = 0; i < cores; ++i) {
		pool.emplace_back(std::thread{ multiGenerate, this, delta, GLOBAL, i});
	}

	for (auto& t : pool) {
		t.join();
	}
	
	//PRINT
	int size = 0;
	for (int i = 0; i < 32; ++i) {
		for (int j = 0; j < 32; ++j) {
			for (int k = 0; k < 32; ++k) {
				std::vector<photon> v = globalMap.at(i, j, k);
				size += v.size();
			}
		}
	}
	storedGlobal = size;
	
}

void camera::generateCausticPhotonMap()
{
	unsigned cores = (MODE == SINGLE_THREAD) ? 1 : std::thread::hardware_concurrency();
	int delta = (int)floor(causticNr / cores);

	std::vector<std::thread> pool;

	for (unsigned i = 0; i < cores; ++i) {
		pool.emplace_back(std::thread{ multiGenerate, this, delta, CAUSTIC, i });
	}

	for (auto& t : pool) {
		t.join();
	}

	//PRINT
	int size = 0;
	int size2 = 0;
	for (int i = 0; i < 32; ++i) {
		for (int j = 0; j < 32; ++j) {
			for (int k = 0; k < 32; ++k) {
				std::vector<photon> v = causticMap.at(i, j, k);
				size += v.size();
			}
		}
	}
	storedCaustic = size;
}

void camera::addToMap(int TYPE, glm::vec3 pos, glm::vec3 dir, float f) {
	photon newPhoton(pos,dir,f);
	int x = static_cast<int>(round(newPhoton.startPoint.x)) + offsetVec.x;
	int y = static_cast<int>(round(newPhoton.startPoint.y)) + offsetVec.y;
	int z = static_cast<int>(round(newPhoton.startPoint.z)) + offsetVec.z;
	OCT_MUTEX.lock();
	if (TYPE == GLOBAL) {
		std::vector<photon> &photons = globalMap(x, y, z);
		photons.emplace_back(newPhoton);
	}
	else {
		std::vector<photon> &photons = causticMap(x, y, z);
		photons.push_back(newPhoton);
	}
	OCT_MUTEX.unlock();
}

int camera::getAndUpdateCurrentLine()
{
	LINE_MUTEX.lock();
	int L = currentLine;
	++currentLine;
	LINE_MUTEX.unlock();
	return L;
}

void camera::bouncePhoton(ray & r, int depth, int TYPE)
{
	float flux;
	if(TYPE == GLOBAL)  flux = (globalNr > 0) ? LIGHTWATT / globalNr : 0;
	else flux = (causticNr > 0) ? LIGHTWATT / causticNr : 0;

	auto intersection = findClosestIntersection(r);
	if (intersection.second.first == nullptr) {
		std::cout << "ERROR" << std::endl;
		return;
	}
	
	if (intersection.second.first->isEmitter())
	{
		glm::vec3 pos = intersection.first;
		glm::vec3 dir = glm::normalize(intersection.first - (glm::vec3)r.getStartVec());
		addToMap(TYPE, pos, dir, flux);
		return;
	}
	else
	{
		glm::vec3 normal = intersection.second.first->isImplicit() ?
			((intersection.first - intersection.second.first->getPosition()) / intersection.second.first->getRadius())
			: intersection.second.second->getNormal();

		if (intersection.second.first->getSurfProperty() == DIFFUSE || intersection.second.first->getSurfProperty() == ORENNAYAR)
		{
			if (TYPE == CAUSTIC)
			{
				glm::vec3 pos = intersection.first;
				glm::vec3 dir = glm::normalize(intersection.first - (glm::vec3)r.getStartVec());
				addToMap(TYPE, pos, dir, flux);
				return;
			}
			float cosTheta = distribution(generator);
			float sidPhi = distribution(generator);
			float russianRoulette = distribution(generator);
			glm::vec3 X(0.0f);
			glm::vec3 Y(0.0f);
			glm::vec3 I = glm::normalize((glm::vec3)r.getStartVec() - (glm::vec3)r.getEndVec());
			getLocalCoordSystem(normal, I, X, Y);

			glm::vec3 sample = sampleHemisphere(cosTheta, sidPhi);

			glm::vec3 worldSample = localToWorld(X, Y, normal, sample, intersection.first);

			glm::vec3 directionWorld = glm::normalize(worldSample - intersection.first);

			float rho;
			if (intersection.second.first->isImplicit())
			{
				rho = (float)(intersection.second.first->getRho() / PI);
			}
			else
			{
				rho = (float)(intersection.second.second->getRho() / PI);
			}
			float Fr = rho;
			if (intersection.second.first->getSurfProperty() == ORENNAYAR)
			{
				float standardDev = 0.3f * 0.3f;
				float A = 1.0f - (standardDev / (2.0f*(standardDev + 0.33f)));
				float B = (0.45f * standardDev) / (standardDev + 0.09f);
				glm::vec3 dirIn = ((glm::vec3)r.getStartVec() - intersection.first);
				glm::vec3 projectedIn = glm::normalize(dirIn - (glm::dot(dirIn, normal) * normal));
				float thetaIN = glm::dot(normal, dirIn);
				thetaIN = acosf(thetaIN);
				float thetaOUT = acosf(cosTheta);
				float alpha = std::max(thetaIN, thetaOUT);
				float beta = std::min(thetaIN, thetaOUT);

				glm::vec3 projectedOut = glm::normalize(directionWorld - (glm::dot(directionWorld, normal) * normal));
				float deltaPhi = glm::dot(projectedIn, projectedOut);
				Fr = rho * (A + B * std::max(0.0f, deltaPhi * sinf(alpha) * sinf(beta)));
			}

			if (depth > 0 && (depth >= MAXDEPTH || russianRoulette < Fr))
			{
				glm::vec3 pos = intersection.first;
				glm::vec3 dir = glm::normalize(intersection.first - (glm::vec3)r.getStartVec());
				addToMap(TYPE, pos, dir, flux);
				return;
			}
			vertex v1 = vertex(intersection.first + directionWorld * 0.00001f, 1.0f);
			vertex v2 = vertex(worldSample, 1.0f);
			ray outRay(v1, v2);
			addToMap(TYPE, intersection.first, directionWorld, flux);
			return bouncePhoton(outRay, depth+1, TYPE);

		}
		else if (intersection.second.first->getSurfProperty() == MIRROR)
		{
			ray rMirror = mirror(intersection, r, normal);
			return bouncePhoton(rMirror, depth, TYPE);
		}
		else if (intersection.second.first->getSurfProperty() == REFRACT)
		{
			vertex v(0.0f);
			ray rayReflect(v, v);
			ray rayRefract(v, v);
			float R, T;
			bool refraction = refract(intersection, r, normal, rayReflect, rayRefract, R, T);
			if (!refraction)
			{
				return bouncePhoton(rayReflect, depth, TYPE);
			}
			else
			{
				float rand = distribution(generator);
				if (rand < R)
				{
					return bouncePhoton(rayReflect, depth, TYPE);
				}
				else
				{
					return bouncePhoton(rayRefract, depth, TYPE);
				}
			}
		}
		else
		{
			std::cout << "ERROR" << std::endl;
			return; // wtf
		}
	}
	

}
