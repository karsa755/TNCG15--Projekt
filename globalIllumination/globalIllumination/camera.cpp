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

void camera::switchEye(glm::vec3 & e) {
	currentEye = &e;
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


int camera::getWidht()
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

double camera::getBrightest(int i)
{
	return brightest[i];
}

void camera::setBrightest(int v, int i)
{
	brightest[i] = v;
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

void camera::setPrintReady(int row, int i)
{
	timeToPrint[i].first = true;
	timeToPrint[i].second = row;
}

bool camera::isReadyToPrint()
{
	return (timeToPrint[0].first && timeToPrint[1].first && timeToPrint[2].first && timeToPrint[3].first);
}

void camera::getPrintRows(int toReturn[4])
{
	toReturn[0] = timeToPrint[0].second;
	toReturn[1] = timeToPrint[1].second;
	toReturn[2] = timeToPrint[2].second;
	toReturn[3] = timeToPrint[3].second;
}

void camera::setRenderType(int type)
{
	renderType = type;
}

void camera::clearReadyToPrint()
{
	timeToPrint[0].first = false;
	timeToPrint[1].first = false;
	timeToPrint[2].first = false;
	timeToPrint[3].first = false;
}

void camera::printContext()
{
	std::cout << "Rendering " << width << "x" << height << std::endl;
	std::cout << "Branching Factor: " << FACTOR << std::endl;
	std::cout << "Number Shadow Rays: " << SHADOWRAYS << std::endl;
	std::cout << "Max Depth: " << MAXDEPTH << std::endl;
	std::cout << "Init ray: " << initRAY << std::endl << std::endl;
}

void camera::addIntensity(glm::dvec3 val, int th)
{
	intensities[th] += val;

}

void camera::setNewMaxIntensity(glm::dvec3 val, int th)
{
	intensityRange[th].x = std::max(val.x, intensityRange[th].x);
	intensityRange[th].y = std::max(val.y, intensityRange[th].y);
	intensityRange[th].z = std::max(val.z, intensityRange[th].z);
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

	if (intersection.second.second != nullptr && !intersection.second.first->isImplicit() && intersection.second.second->isEmitter) {
		//hitting light source
		//std::cout << "TO LIGHTSOURCE" << std::endl;
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
			//std::cout << "D" << std::endl;
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
				outRay.setImportance(r.getImportance() * cosTheta);
				finalColor += (double)cosTheta * castRay(outRay, depth + 1); //WTF Why not Divide with pdf?
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
		else if(intersection.second.first->getSurfProperty() == ORENNAYAR) //have to check if this is correct
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
					std::vector<photon> &currentPhotonsCaustic = causticMap(x+i,y+j,z+k);
					std::vector<photon> &currentPhotonsGlobal = globalMap(x + i, y + j, z + k);
					for (photon p : currentPhotonsCaustic)
					{
						float dist = glm::distance(intersection.first, p.startPoint);
						if (dist < r0Caustic)
						{
							radiance += (rho * p.flux) / ((float)PI * dist);
						}
						
					}
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
		color indirLight = calcIndirectLight(r, 0);
		return c * ((double)radiance + directLight) + indirLight;
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
		return color(1.0, 1.0, 1.0) * (double)radiance;
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
			//std::cout << "D" << std::endl;
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
				outRay.setImportance(r.getImportance() * cosTheta);
				finalColor += (double)cosTheta * calcIndirectLight(outRay, depth + 1) * rho; //WTF Why not Divide with pdf?
			}
			finalColor /= ((double)N * PDF);
			return c * finalColor;
		}
		else if (intersection.second.first->getSurfProperty() == MIRROR)
		{

			ray rMirror = mirror(intersection, r, normal);
			return calcIndirectLight(rMirror, depth);
		}
		else if (intersection.second.first->getSurfProperty() == ORENNAYAR) //have to check if this is correct
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

void multi(camera *c, int dims[4], int thr) {
	const float pixelWidth = 2.0 / c->getWidht();
	const float pixelHeight = 2.0 / c->getHeight();
	const float deltaPW = pixelWidth / 2.0;
	const float deltaPH = pixelHeight / 2.0;
	int renderAs = c->getRenderType();

	for (int i = dims[0]; i < dims[2]; ++i) {
		for (int j = dims[1]; j < dims[3]; ++j) {

			color col(0.0);
			for (int num = 0; num < c->getInitRay(); ++num)
			{
				float newDeltaPW = pixelWidth * c->distribution(c->generator);
				float newDeltaPH = pixelHeight * c->distribution(c->generator);
				float y = (1.0 - i * pixelWidth) - newDeltaPW;
				float z = (j * pixelHeight - 1.0) + newDeltaPH;
				float x = 0.0;

				vertex s = vertex(c->getCurrentEye()->x, c->getCurrentEye()->y, c->getCurrentEye()->z, 1.0);
				vertex e = vertex(x, y, z, 1.0);

				//Create ray between eye and pixelplane
				ray r = ray(s, e);
				r.setImportance(1.0);

				//cast
				if (renderAs == PHOTONMAPPING)
				{
					//cast photon map ray
					col += c->photonMapRender(r);
				}
				else
				{
					col += c->castRay(r, 0);
				}
				
			}
			col /= (double)c->getInitRay();
			c->addIntensity(col, thr);

			double m = std::max(std::max(col.x, col.y), col.z);
			if (m > c->getBrightest(thr) && m < LIGHTWATT / (2.0 * PI * AREA)) //ASUMES WHITE COLORED LIGHT
				c->setBrightest(m,thr);

			c->setNewMaxIntensity(col, thr);
			c->setPixelValue(col,i,j);
		}
		
		c->setPrintReady((i + 1) - dims[0], thr);
		
		if (c->isReadyToPrint()) {
			c->clearConsole();
			int statuses[4];
			c->getPrintRows(statuses);
			c->printContext();

			for (int q = 0; q < 4; ++q) {
				std::cout << "(Thread " << q << "): " << statuses[q] << " of " << dims[2] - dims[0] << " complete" << std::endl;
			}
			c->threadPrinter = (c->threadPrinter >= 3) ? 0 : c->threadPrinter+1;

		}
	}
}

double camera::sigMoidNormalize(double pixVal, double alpha, double beta)
{
	double res = 1.0 / ( 1.0 + std::exp(-((pixVal - beta) / alpha)) );
	return res;
}

void camera::render() {

	const float pixelWidth = 2.0 / width;
	const float pixelHeight = 2.0 / height;
	const float deltaPW = pixelWidth / 2.0;
	const float deltaPH = pixelHeight / 2.0;

	int debugCounter = 0;

	if (MODE == SINGLE_THREAD) {
		for (int i = 0; i < width; i++) {		
			for (int j = 0; j < height; ++j) {
				 

				float newDeltaPW = pixelWidth * distribution(generator);
				float newDeltaPH = pixelHeight * distribution(generator);
				float y = (1.0 - i * pixelWidth) - newDeltaPW;
				float z = (j * pixelHeight - 1.0) + newDeltaPH;
				float x = 0.0;

				vertex s = vertex(currentEye->x, currentEye->y, currentEye->z, 1.0);
				vertex e = vertex(x, y, z, 1.0);

				//Create ray between eye and pixelplane
				ray r = ray(s, e);
				r.setImportance(1.0);

				//cast
				color c;
				c = castRay(r, 0);

				double m = std::max(std::max(c.x,c.y),c.z); 
				
				if (m > brightest[0]) //&& m < LIGHTWATT) //ASUMES WHITE COLORED LIGHT
					brightest[0] = m;

				image[i][j].setIntensity(c);
			}
			
			clearConsole();
			printContext();
			std::cout << "Progress: " << (i + 1) << " of " << width << " rows complete" << std::endl;
		 }
	}
	else if (MODE == MULTI_THREAD) {

		
		int hx = width/2;
		int hy = height/2;

		int a_dims[4] = { 0,0,hx,hy };
		int b_dims[4] = { hx,0,width,hy };
		int c_dims[4] = { 0,hy,hx,height };
		int d_dims[4] = { hx,hy,width,height};

		std::cout << "Started" << std::endl;
		
		int renderAs = getRenderType();
		
		if (renderAs == PHOTONMAPPING)
		{
			//generate photon maps 
			generateGlobalPhotonMap();
			generateCausticPhotonMap();
		}

		
		std::vector<std::thread> pool;
		pool.emplace_back(std::thread{ multi, this, a_dims, 0 });
		pool.emplace_back(std::thread{ multi, this, b_dims, 1 });
		pool.emplace_back(std::thread{ multi, this, c_dims, 2 });
		pool.emplace_back(std::thread{ multi, this, d_dims, 3 });

		for (auto& t : pool) {
			t.join();
		}
		
	}
	

	std::cout << "Number of failed pixels: " << debugCounter << std::endl;

	double bMax = std::max(std::max(std::max(brightest[0], brightest[1]), brightest[2]), brightest[3]);
	//color beta = (intensities[0] + intensities[1] + intensities[2] + intensities[3]) / (double)(width*height);
	color range = color(0.0);
	range.x = std::max(std::max(intensityRange[0].x, intensityRange[1].x), std::max(intensityRange[2].x, intensityRange[3].x));
	range.y = std::max(std::max(intensityRange[0].y, intensityRange[1].y), std::max(intensityRange[2].y, intensityRange[3].y));
	range.z = std::max(std::max(intensityRange[0].z, intensityRange[1].z), std::max(intensityRange[2].z, intensityRange[3].z));
	double rangeMAX = std::max(std::max(range.x, range.y), range.z);
	//double betaMAX = std::max(std::max(beta.x, beta.y), beta.z);
	//bMax = (bMax + LIGHTWATT) / 2.0;
	//bMax = LIGHTWATT;
	//bMax = 1.0;
	if (rangeMAX < 1.0)
	{
		//rangeMAX = 1.0;
	}
	std::cout << "Trying whatever normalizer for light..." << std::endl;
	std::cout << rangeMAX << " " << bMax << std::endl;
	
	//Write
	FILE *f = fopen("out.ppm", "wb");
	fprintf(f, "P6\n%i %i 255\n", width, height);
	for (int y = height; y > 0; y--) {
		for (int x = 0; x < width; x++) {

			
			//double sigmoidX = std::pow(sigMoidNormalize(image[x][y].getIntensity().x, rangeMAX, rangeMAX / 2.0) ,1.0);
			//double sigmoidY = std::pow(sigMoidNormalize(image[x][y].getIntensity().y, rangeMAX, rangeMAX / 2.0), 1.0);
			//double sigmoidZ = std::pow(sigMoidNormalize(image[x][y].getIntensity().z, rangeMAX, rangeMAX / 2.0), 1.0);
			fputc(std::pow(std::min((image[x][y].getIntensity().x), 1.0), 0.5) * 255, f);   // 0 .. 255
			fputc(std::pow(std::min((image[x][y].getIntensity().y), 1.0), 0.5) * 255, f); // 0 .. 255
			fputc(std::pow(std::min((image[x][y].getIntensity().z), 1.0), 0.5) * 255, f);  // 0 .. 255


			//fputc(sigmoidX * 255, f);   // 0 .. 255
			//fputc(sigmoidY * 255, f); // 0 .. 255
			//fputc(sigmoidZ * 255, f);  // 0 .. 255
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
	//glm::vec3 rayDir = glm::normalize(r.getEndVec() - r.getStartVec());
	//if (glm::distance(intersection.first + 0.0001f * rayDir, intersection.second.first->getPosition())
	//	< intersection.second.first->getRadius())
	if (std::fabs(thetaIN) >= (float)PI / 2.0f)
	{
		//inside sphere
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
			for (int q = 0; q < 4; ++q) {
				std::cout << "Thread (" << q << "): " << (i + 1) << " of " << d << " done" << std::endl;
			}
			c->threadPrinter = (c->threadPrinter >= 3) ? 0 : c->threadPrinter+1;
		}
		if (t == CAUSTIC && (i + 1) % (int)floor(d / (10)) == 0 && (c->threadPrinter == thr)) {
			c->clearConsole();
			std::cout << "(CAUSTIC) Emitting " << c->causticNr << " Photons" << std::endl;
			for (int q = 0; q < 4; ++q) {
				std::cout << "Thread (" << q << "): " << (i + 1) << " of " << d << " done" << std::endl;
			}
			c->threadPrinter = (c->threadPrinter >= 3) ? 0 : c->threadPrinter + 1;
		}
	}
}

void camera::merge(std::vector<photon>& v) {
	int x, y, z;
	for (auto it = v.begin(); it != v.end(); ++it) {
		x = static_cast<int>(round(it->startPoint.x)) + offsetVec.x;
		y = static_cast<int>(round(it->startPoint.y)) + offsetVec.y;
		z = static_cast<int>(round(it->startPoint.z)) + offsetVec.z;
		std::vector<photon> photons = globalMap.at(x, y, z);
		photons.push_back(*it);
		globalMap(x, y, z) = photons;
	}
}

void camera::generateGlobalPhotonMap()
{
	
	int delta = (int)floor(globalNr / 4);
	
	std::vector<std::thread> pool;
	pool.emplace_back(std::thread{ multiGenerate, this, delta, GLOBAL, 0 });
	pool.emplace_back(std::thread{ multiGenerate, this, delta, GLOBAL, 1 });
	pool.emplace_back(std::thread{ multiGenerate, this, delta, GLOBAL, 2 });
	pool.emplace_back(std::thread{ multiGenerate, this, delta, GLOBAL, 3 });

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
	std::cout << std::endl << "Number of Global Photons stored: " << size << std::endl << std::endl;
	
}

void camera::generateCausticPhotonMap()
{
	int delta = (int)floor(causticNr / 4);

	std::vector<std::thread> pool;
	pool.emplace_back(std::thread{ multiGenerate, this, delta, CAUSTIC, 0 });
	pool.emplace_back(std::thread{ multiGenerate, this, delta, CAUSTIC, 1 });
	pool.emplace_back(std::thread{ multiGenerate, this, delta, CAUSTIC, 2 });
	pool.emplace_back(std::thread{ multiGenerate, this, delta, CAUSTIC, 3 });

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
				std::vector<photon> v2 = globalMap.at(i, j, k);
				size2 += v2.size();
			}
		}
	}
	std::cout << std::endl << "Number of Global Photons stored: " << size2 << std::endl;
	std::cout << std::endl << "Number of caustic Photons stored: " << size << std::endl << std::endl;
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
