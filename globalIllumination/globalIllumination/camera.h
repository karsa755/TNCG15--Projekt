#pragma once

#include <iostream>
#include "octree.h"
#include <random>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <future>
#include <mutex>
#include <array>

#include "definition.h"
#include "pixel.h"
#include "object.h"
#include "polyModel.h"
#include "lightSource.h"

class camera
{
public:
	camera(std::vector<object*> ol);
	~camera();

	triangle getLightSource();
	void getLocalCoordSystem(const glm::vec3 &Z, const glm::vec3 &I, glm::vec3 &X, glm::vec3 &Y);
	glm::vec3 localToWorld(const glm::vec3 &X, const glm::vec3 &Y, const glm::vec3 &Z, const glm::vec3 & v, const glm::vec3 &t);
	glm::vec3 worldToLocal(const glm::vec3 &X, const glm::vec3 &Y, const glm::vec3 &Z, const glm::vec3 & v);
	void setBranchFactor(int f);
	void setShadowRays(int n);
	void setDepth(int d);
	double sigMoidNormalize(double pixVal, double range, double beta);
	int getWidth();
	int getHeight();
	const glm::vec3* getCurrentEye();
	void setPixelValue(color c, int i, int j);
	void setRenderingMode(int mode);
	void clearConsole();
	void setRenderType(int type);
	void printContext();
	std::pair<glm::vec3, std::pair<object*, triangle*>> findClosestIntersection(ray &r);
	color castRay(ray &r, int depth);
	color photonMapRender(ray &r);
	color calcIndirectLight(ray &r, int depth);
	void setInitRay(int ray);
	int getInitRay();
	void generateGlobalPhotonMap();
	void generateCausticPhotonMap();
	void bouncePhoton(ray &r, int depth, int TYPE);
	void addToMap(int TYPE, glm::vec3 pos, glm::vec3 dir, float flux);

	int getAndUpdateCurrentLine();

	ray mirror(const std::pair<glm::vec3, std::pair<object*, triangle*>> &intersection, ray r, glm::vec3 normal);
	bool refract(const std::pair<glm::vec3, std::pair<object*, triangle*>> &intersection, ray r, glm::vec3 normal, 
		ray &reflect, ray &refract, float &Rnew, float &Tnew);
	void render();
	int getRenderType();
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;

	lightSource *photonSource;
	const glm::vec3 offsetVec = glm::vec3(10, 10, 10);
	Octree<std::vector<photon>> causticMap;
	Octree<std::vector<photon>> globalMap;

	int threadPrinter = 0;
	int storedGlobal;
	int storedCaustic;

	const static int globalNr = 50000;
	const static int causticNr = 150000;
	int MODE = SINGLE_THREAD;

	
private:
	void findLightSource();
	const static int width = 200;
	const static int height = 200;
	const static int boxSize = 32;
	
	const glm::vec3 * currentEye;
	const glm::vec3 position = glm::vec3(0.0,0.0,0.0);
	const glm::vec3 eye1 = glm::vec3(-2.0,0.0,0.0);
	const glm::vec3 eye2 = glm::vec3(-1.0, 0.0, 0.0);
	const glm::vec3 plane[4] = { glm::vec3(0.0,-1.0,-1.0), glm::vec3(0.0,1.0,-1.0), glm::vec3(0.0,1.0,1.0), glm::vec3(0.0,-1.0,1.0) };
	pixel image[width][height] = { pixel() };
	std::vector<object*> objects;
	triangle light;
	
	int FACTOR = 1;
	int SHADOWRAYS = 1;
	int MAXDEPTH = 1;
	int initRAY = 1;
	int renderType;

	int currentLine = 0;

	std::mutex OCT_MUTEX;
	std::mutex LINE_MUTEX;
};

