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
	std::cout << "Max Depth: " << MAXDEPTH << std::endl << std::endl;
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
	std::vector<glm::vec3> lightSamples;
	double rho;
	if (intersection.second.first->isImplicit())
	{
		rho = intersection.second.first->getRho();
	}
	else
	{
		rho = intersection.second.second->getRho();
	}

	for (int i = 0; i < SHADOWRAYS; ++i)
	{
		if (i == 0)
		{
			lightSamples.emplace_back(midPoint);
		}
		else
		{
			float u = distribution(generator);
			float v = (1 - u) * distribution(generator);
			lightSamples.emplace_back(lightSource.sampleTriangle(u, v));
		}

	}

	if (intersection.second.first == nullptr) {
		std::cout << "ERROR" << std::endl;
		return color(0.0,0.0,0.0);
	}

	if (intersection.second.second != nullptr && !intersection.second.first->isImplicit() && intersection.second.second->isEmitter) {
		//hitting light source
		//std::cout << "TO LIGHTSOURCE" << std::endl;
		color ret = intersection.second.second->getSurfaceColor();
		return (double)LIGHTWATT* ret;
	}
	
	if (depth >= MAXDEPTH) {
		//shadow rays n' stuff
		color dirLight = { 1.0, 1.0, 1.0 };

		glm::vec3 normal = intersection.second.first->isImplicit() ?
			((intersection.first - intersection.second.first->getPosition()) / intersection.second.first->getRadius())
			: intersection.second.second->getNormal();
		
		vertex startPoint = { intersection.first + (normal * 0.001f),1.0f };
		double lightHits = 0.0;
		for (int i = 0; i < SHADOWRAYS; ++i)
		{
			vertex n = vertex(lightSamples.at(i), 1.0f);
			ray toLight(startPoint, n);

			auto closest = findClosestIntersection(toLight);
			if (!closest.second.first->isImplicit() && closest.second.second->isEmitter) {
				lightHits += (1.0f * std::max(0.0f, glm::dot(normal, glm::normalize(lightSamples.at(i) - (glm::vec3)startPoint))));
			}
		}

		return (rho*(double)LIGHTWATT *color(lightHits, lightHits, lightHits))  / (1.0 * (double)AREA * (double)SHADOWRAYS);
	}
	else {
		//recursive call
		int N = FACTOR;
		glm::vec3 X;
		glm::vec3 Y;
		glm::vec3 I = intersection.first - (glm::vec3)r.getStartVec();
		glm::vec3 Z = intersection.second.first->isImplicit() ? 
								((intersection.first - intersection.second.first->getPosition()) / intersection.second.first->getRadius()) 
								: intersection.second.second->getNormal();

		getLocalCoordSystem(Z,I,X,Y);

		color finalColor(0.0, 0.0, 0.0);
		vertex startPoint = { intersection.first + (Z * 0.001f),1.0f };

		double lightHits = 0.0;
		for (int i = 0; i < SHADOWRAYS; ++i)
		{
			vertex n = vertex(lightSamples.at(i), 1.0f);
			ray toLight(startPoint, n);

			auto closest = findClosestIntersection(toLight);
			if (!closest.second.first->isImplicit() && closest.second.second->isEmitter) {
				lightHits += ( 1.0f * std::max(0.0f, glm::dot(Z, glm::normalize(lightSamples.at(i) - (glm::vec3)startPoint))) );
			}
		}
		
		if (intersection.second.first->getSurfProperty() == DIFFUSE)
		{
			//std::cout << "D" << std::endl;
			double PDF = 1.0 / (2.0*PI);
			
			for (int n = 0; n < N; ++n) {
				float cosTheta = distribution(generator);
				float sidPhi = distribution(generator);
				glm::vec3 sample = sampleHemisphere(cosTheta, sidPhi);

				glm::vec3 worldSample = localToWorld(X, Y, Z, sample);
				vertex v1 = vertex(intersection.first + worldSample * 0.1f, 1.0f);
				vertex v2 = vertex(worldSample, 1.0f);
				ray outRay(v1, v2);
				outRay.setImportance(r.getImportance() * cosTheta);
				finalColor += (double)cosTheta * castRay(outRay, depth + 1) / PDF; //WTF Why not Divide with pdf?
			}
			finalColor /= (double)N;
			//finalColor *= rho;
			color c;

			if (intersection.second.first->isImplicit()) {
				c = intersection.second.first->getColor();
			}
			else {
				c = intersection.second.second->getSurfaceColor();
			}
			color dirLight = { lightHits, lightHits, lightHits };

			color finC = rho * ( ( (double)LIGHTWATT * dirLight / ((double)SHADOWRAYS*AREA)) + (finalColor)) * c;
			return finC;
		}
		else if (intersection.second.first->getSurfProperty() == MIRROR)
		{
			if (intersection.first.x < 0.01f) {
				std::cout << "Fuck" << std::endl;
				return color(1.0, 0.0, 0.0) * LIGHTWATT;
			}
			vertex dir = vertex(intersection.first, 1.0f) - r.getStartVec();
			dir.w = 1.0f;
			vertex reflectDir = glm::reflect(dir, vertex(Z, 1.0f));
			vertex startPt = vertex(intersection.first + (glm::vec3)reflectDir*0.01f, 1.0f);
			//vertex startPt  = r.getEndVec();

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


void multi(camera *c, int dims[4], int thr) {
	const float pixelWidth = 2.0 / c->getWidht();
	const float pixelHeight = 2.0 / c->getHeight();
	const float deltaPW = pixelWidth / 2.0;
	const float deltaPH = pixelHeight / 2.0;

	for (int i = dims[0]; i < dims[2]; ++i) {
		for (int j = dims[1]; j < dims[3]; ++j) {
			float y = (1.0 - i * pixelWidth) - deltaPW;
			float z = (j * pixelHeight - 1.0) + deltaPH;
			float x = 0.0;

			vertex s = vertex(c->getCurrentEye()->x, c->getCurrentEye()->y, c->getCurrentEye()->z, 1.0);
			vertex e = vertex(x, y, z, 1.0);

			//Create ray between eye and pixelplane
			ray r = ray(s, e);
			r.setImportance(1.0);

			//cast
			color col;
			col = c->castRay(r, 0);

			c->addIntensity(col, thr);

			double m = std::max(std::max(col.x, col.y), col.z);
			if (m > c->getBrightest(thr)) // && m < LIGHTWATT) //ASUMES WHITE COLORED LIGHT
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
		}
	}
}

double camera::sigMoidNormalize(double pixVal, double range, double beta)
{
	double res = 1.0 / ( 1.0 + std::exp(-((pixVal - beta) / range)) );
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
				float y = (1.0 - i * pixelWidth) - deltaPW;
				float z = (j * pixelHeight - 1.0) + deltaPH;
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
	//color range = color(0.0);
	//range.x = std::max(std::max(intensityRange[0].x, intensityRange[1].x), std::max(intensityRange[2].x, intensityRange[3].x));
	//range.y = std::max(std::max(intensityRange[0].y, intensityRange[1].y), std::max(intensityRange[2].y, intensityRange[3].y));
	//range.z = std::max(std::max(intensityRange[0].z, intensityRange[1].z), std::max(intensityRange[2].z, intensityRange[3].z));
	//double rangeMAX = std::max(std::max(range.x, range.y), range.z);
	//double betaMAX = std::max(std::max(beta.x, beta.y), beta.z);
	//bMax = (bMax + LIGHTWATT) / 2.0;
	//bMax = LIGHTWATT;
	//bMax = 1.0;
	std::cout << "Trying whatever normalizer for light..." << std::endl;
	
	//Write
	FILE *f = fopen("out.ppm", "wb");
	fprintf(f, "P6\n%i %i 255\n", width, height);
	for (int y = height; y > 0; y--) {
		for (int x = 0; x < width; x++) {

			
			//double sigmoidX = std::pow(sigMoidNormalize(image[x][y].getIntensity().x, range.x, range.x / 2.0) ,1.0);
			//double sigmoidY = std::pow(sigMoidNormalize(image[x][y].getIntensity().y, range.y, range.y / 2.0), 1.0);
			//double sigmoidZ = std::pow(sigMoidNormalize(image[x][y].getIntensity().z, range.z, range.z / 2.0), 1.0);

			if (image[x][y].getIntensity().x == LIGHTWATT)
			{
				fputc(255, f);   // 0 .. 255
				fputc(255, f);
				fputc(255, f);
			}
			else
			{
				fputc(std::pow(std::min((image[x][y].getIntensity().x / bMax), 1.0), 0.5) * 255, f);   // 0 .. 255
				fputc(std::pow(std::min((image[x][y].getIntensity().y / bMax), 1.0), 0.5) * 255, f); // 0 .. 255
				fputc(std::pow(std::min((image[x][y].getIntensity().z / bMax), 1.0), 0.5) * 255, f);  // 0 .. 255
			}


			//fputc(sigmoidX * 255, f);   // 0 .. 255
			//fputc(sigmoidY * 255, f); // 0 .. 255
			//fputc(sigmoidZ * 255, f);  // 0 .. 255
		}
	}
	fclose(f);
}

camera::~camera()
{
}
