#pragma once

#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <stack>
#include <Windows.h>

#include "definition.h"
#include "pixel.h"
#include "object.h"
#include "polyModel.h"

class camera
{
public:
	camera(std::vector<object*> ol);
	~camera();

	void switchEye(glm::vec3 & e);
	triangle getLightSource();
	void getLocalCoordSystem(const glm::vec3 &Z, const glm::vec3 &I, glm::vec3 &X, glm::vec3 &Y);
	glm::vec3 localToWorld(const glm::vec3 &X, const glm::vec3 &Y, const glm::vec3 &Z, const glm::vec3 & v);
	glm::vec3 worldToLocal(const glm::vec3 &X, const glm::vec3 &Y, const glm::vec3 &Z, const glm::vec3 & v);
	void setBranchFactor(int f);
	void setShadowRays(int n);
	void setDepth(int d);
	double sigMoidNormalize(double pixVal, double range, double beta);
	int getWidht();
	int getHeight();
	const glm::vec3* getCurrentEye();
	void setPixelValue(color c, int i, int j);
	double getBrightest(int i);
	void setBrightest(int v, int i);
	void setRenderingMode(int mode);
	void clearConsole();
	void setPrintReady(int row, int i);
	bool isReadyToPrint();
	void getPrintRows(int toReturn[4]);
	void clearReadyToPrint();
	void printContext();
	void addIntensity(glm::dvec3 val, int th);
	void setNewMaxIntensity(glm::dvec3 val, int th);
	std::pair<glm::vec3, std::pair<object*, triangle*>> findClosestIntersection(ray &r);
	color castRay(ray &r, int depth);

	void render();
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;

private:
	void findLightSource();
	const static int width = 100;
	const static int height = 100;
	const glm::vec3 * currentEye;
	const glm::vec3 position = glm::vec3(0.0,0.0,0.0);
	const glm::vec3 eye1 = glm::vec3(-2.0,0.0,0.0);
	const glm::vec3 eye2 = glm::vec3(-1.0, 0.0, 0.0);
	const glm::vec3 plane[4] = { glm::vec3(0.0,-1.0,-1.0), glm::vec3(0.0,1.0,-1.0), glm::vec3(0.0,1.0,1.0), glm::vec3(0.0,-1.0,1.0) };
	pixel image[width][height] = { pixel() };
	std::vector<object*> objects;
	triangle lightSource;
	
	double brightest[4] = { 1.0, 1.0, 1.0, 1.0 };
	glm::dvec3 intensities[4] = { glm::dvec3(0.0) };
	glm::dvec3 intensityRange[4] = { glm::dvec3(0.0) };
	std::pair<bool, int> timeToPrint[4] = { std::pair<bool,int>(false,0) };
	
	int FACTOR = 1;
	int SHADOWRAYS = 1;
	int MAXDEPTH = 1;

	int MODE = SINGLE_THREAD;

};

