#include <iostream>
#include "definition.h"
#include "camera.h"
#include "polyModel.h"
#include "implicitModel.h"

#include <chrono>


void createVerts(vertex floorVertices[6], vertex roofVertices[6], vertex lightVerts[3], vertex tetraVerts[4]);
polyModel createFloor(vertex fv[], double rho, int TYPE);
polyModel createRoof(vertex rv[], double rho, int TYPE);
polyModel createRedWall(vertex rv[], vertex fv[], double rho, int TYPE);
polyModel createGreenWall(vertex rv[], vertex fv[], double rho, int TYPE);
polyModel createBlueWall(vertex rv[], vertex fv[], double rho, int TYPE);
polyModel createYellowWall(vertex rv[], vertex fv[], double rho, int TYPE);
polyModel createPurpleWall(vertex rv[], vertex fv[], double rho, int TYPE);
polyModel createCyanWall(vertex rv[], vertex fv[], double rho, int TYPE);
polyModel createLightSource(vertex lv[]);
polyModel createTetra(vertex tv[], double rho, int TYPE);

int main(int, char*[])
{
	std::vector<object*> objectList;
	vertex fv[6];
	vertex rv[6];
	vertex lv[3];
	vertex tt[4];
	createVerts(fv,rv,lv,tt);

	polyModel roof = createRoof(rv, 0.8, DIFFUSE);
	polyModel floor = createFloor(fv, 0.8, DIFFUSE);
	polyModel rw = createRedWall(rv,fv, 0.8, DIFFUSE);		//back right
	polyModel bw = createBlueWall(rv, fv, 0.8, DIFFUSE);	//right
	polyModel yw = createYellowWall(rv, fv, 0.8, DIFFUSE);	//front left
	polyModel gw = createGreenWall(rv, fv, 0.8, DIFFUSE);	//front right
	polyModel pw = createPurpleWall(rv, fv, 0.8, DIFFUSE);	//left
	polyModel cw = createCyanWall(rv, fv, 0.8, DIFFUSE);	//back left
	polyModel light = createLightSource(lv);

	polyModel tetra = createTetra(tt, 0.8, DIFFUSE);
	implicitModel sphere(1.5f, glm::vec3(6.0f, 2.0f, -3.4f), MIRROR, 1.0, color(1.0,0.8,0.4));

	objectList.push_back(&roof);
	objectList.push_back(&floor);
	objectList.push_back(&rw);
	objectList.push_back(&bw);
	objectList.push_back(&yw);
	objectList.push_back(&gw);
	objectList.push_back(&pw);
	objectList.push_back(&cw);
	objectList.push_back(&sphere);
	objectList.push_back(&tetra);
	objectList.push_back(&light);

	camera *cam = new camera(objectList);


	//CONFIG
	cam->setRenderingMode(MULTI_THREAD);
	cam->setBranchFactor(4);
	cam->setShadowRays(4);
	cam->setDepth(20);
	cam->setInitRay(2);
	


	//STUFF
	std::chrono::milliseconds start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	cam->render();
	std::chrono::milliseconds end_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	int seconds = (int)(end_ms - start_ms).count() / 1000.0;
	int minutes = 0;

	while (true) {
		if (seconds - 60 >= 0) {
			++minutes;
			seconds -= 60;
		}
		else {
			break;
		}
	}

	std::cout << "Render Time: " << minutes << "m" << seconds << "s" << std::endl;

	return 0;
}



void createVerts(vertex floorVertices[6], vertex roofVertices[6], vertex lightVerts[3], vertex tetraVerts[4]) {

	floorVertices[0] = vertex(0.0f, 6.0f, -5.0f, 1.0f);
	floorVertices[1] = vertex(-3.0f, 0.0f, -5.0f, 1.0f);
	floorVertices[2] = vertex(0.0f, -6.0f, -5.0f, 1.0f);
	floorVertices[3] = vertex(10.0f, 6.0f, -5.0f, 1.0f);
	floorVertices[4] = vertex(13.0f, 0.0f, -5.0f, 1.0f);
	floorVertices[5] = vertex(10.0f, -6.0f, -5.0f, 1.0f);

	roofVertices[0] = vertex(0.0f, 6.0f, 5.0f, 1.0f);
	roofVertices[1] = vertex(-3.0f, 0.0f, 5.0f, 1.0f);
	roofVertices[2] = vertex(0.0f, -6.0f, 5.0f, 1.0f);
	roofVertices[3] = vertex(10.0f, 6.0f, 5.0f, 1.0f);
	roofVertices[4] = vertex(13.0f, 0.0f, 5.0f, 1.0f);
	roofVertices[5] = vertex(10.0f, -6.0f, 5.0f, 1.0f);

	lightVerts[0] = vertex(5.5f, 0.0f, 4.0f, 1.0f);
	lightVerts[1] = vertex(4.5f, 1.0f, 4.0f, 1.0f);
	lightVerts[2] = vertex(4.5f, -1.0f, 4.0f, 1.0f);

	tetraVerts[0] = vertex(5.5f, -4.0f, -4.9f, 1.0f);
	tetraVerts[1] = vertex(7.0f, -4.0f, -4.9f, 1.0f);
	tetraVerts[2] = vertex(7.0f, -4.0f, -2.5f, 1.0f);
	tetraVerts[3] = vertex(7.0f, -1.0f, -4.9f, 1.0f);
}


polyModel createFloor(vertex fv[], double rho, int TYPE) {
	vertex *floorVertices = fv;
	std::vector<triangle> sceneList;

	sceneList.push_back(triangle(floorVertices[0], floorVertices[1], floorVertices[2], rho, color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(floorVertices[0], floorVertices[2], floorVertices[5], rho, color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(floorVertices[0], floorVertices[5], floorVertices[3], rho, color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(floorVertices[3], floorVertices[5], floorVertices[4], rho, color(1.0, 1.0, 1.0)));

	polyModel floor(sceneList, glm::vec3(0.0, 0.0, 0.0), TYPE, 1.0);
	return floor;
}

polyModel createRoof(vertex rv[], double rho, int TYPE) {
	vertex *roofVertices = rv;
	std::vector<triangle> sceneList;

	sceneList.push_back(triangle(roofVertices[0], roofVertices[2], roofVertices[1], rho, color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[0], roofVertices[5], roofVertices[2], rho, color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[0], roofVertices[3], roofVertices[5], rho, color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[3], roofVertices[4], roofVertices[5], rho, color(1.0, 1.0, 1.0)));

	polyModel roof(sceneList, glm::vec3(0.0, 0.0, 0.0), TYPE, 1.0);
	return roof;
}

polyModel createRedWall(vertex rv[], vertex fv[], double rho, int TYPE) {
	vertex *floorVertices = fv;
	vertex *roofVertices = rv;
	std::vector<triangle> sceneList;

	sceneList.push_back(triangle(roofVertices[1], floorVertices[2], floorVertices[1], rho, color(1.0, 0.0, 0.0)));
	sceneList.push_back(triangle(floorVertices[2], roofVertices[1], roofVertices[2], rho, color(1.0, 0.0, 0.0)));

	polyModel wall(sceneList, glm::vec3(0.0, 0.0, 0.0), TYPE, 1.0);
	return wall;
}

polyModel createGreenWall(vertex rv[], vertex fv[], double rho, int TYPE) {
	vertex *floorVertices = fv;
	vertex *roofVertices = rv;
	std::vector<triangle> sceneList;

	sceneList.push_back(triangle(floorVertices[5], roofVertices[5], roofVertices[4], rho, color(0.0, 1.0, 0.0)));
	sceneList.push_back(triangle(roofVertices[4], floorVertices[4], floorVertices[5], rho, color(0.0, 1.0, 0.0)));

	polyModel wall(sceneList, glm::vec3(0.0, 0.0, 0.0), TYPE, 1.0);
	return wall;
}

polyModel createBlueWall(vertex rv[], vertex fv[], double rho, int TYPE) {
	vertex *floorVertices = fv;
	vertex *roofVertices = rv;
	std::vector<triangle> sceneList;

	sceneList.push_back(triangle(floorVertices[5], floorVertices[2], roofVertices[5], rho, color(0.0, 0.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[2], roofVertices[5], floorVertices[2], rho, color(0.0, 0.0, 1.0)));

	polyModel wall(sceneList, glm::vec3(0.0, 0.0, 0.0), TYPE, 1.0);
	return wall;
}

polyModel createYellowWall(vertex rv[], vertex fv[], double rho, int TYPE) {
	vertex *floorVertices = fv;
	vertex *roofVertices = rv;
	std::vector<triangle> sceneList;

	sceneList.push_back(triangle(roofVertices[4], floorVertices[3], floorVertices[4], rho, color(1.0, 1.0, 0.0)));
	sceneList.push_back(triangle(roofVertices[3], floorVertices[3], roofVertices[4], rho, color(1.0, 1.0, 0.0)));

	polyModel wall(sceneList, glm::vec3(0.0, 0.0, 0.0), TYPE, 1.0);
	return wall;
}

polyModel createPurpleWall(vertex rv[], vertex fv[], double rho, int TYPE) {
	vertex *floorVertices = fv;
	vertex *roofVertices = rv;
	std::vector<triangle> sceneList;

	sceneList.push_back(triangle(roofVertices[0], floorVertices[0], floorVertices[3], rho, color(1.0, 0.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[0], floorVertices[3], roofVertices[3], rho, color(1.0, 0.0, 1.0)));

	polyModel wall(sceneList, glm::vec3(0.0, 0.0, 0.0), TYPE, 1.0);
	return wall;
}

polyModel createCyanWall(vertex rv[], vertex fv[], double rho, int TYPE) {
	vertex *floorVertices = fv;
	vertex *roofVertices = rv;
	std::vector<triangle> sceneList;

	sceneList.push_back(triangle(roofVertices[0], floorVertices[1], floorVertices[0], rho, color(0.0, 1.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[1], floorVertices[1], roofVertices[0], rho, color(0.0, 1.0, 1.0)));

	polyModel wall(sceneList, glm::vec3(0.0, 0.0, 0.0), TYPE, 1.0);
	return wall;
}

polyModel createLightSource(vertex lv[]) {
	vertex *lightVerts = lv;
	std::vector<triangle> sceneList;

	sceneList.push_back(triangle(lightVerts[1], lightVerts[0], lightVerts[2], 1.0, color(1.0, 1.0, 1.0), true));

	polyModel light(sceneList, glm::vec3(0.0, 0.0, 0.0), DIFFUSE, 1.0);
	return light;
}

polyModel createTetra(vertex tv[], double rho, int TYPE)
{
	vertex *tetraVerts = tv;
	std::vector<triangle> sceneList;

	sceneList.push_back(triangle(tetraVerts[0], tetraVerts[2], tetraVerts[1], rho, color(1.0, 0.0, 0.0)));
	sceneList.push_back(triangle(tetraVerts[3], tetraVerts[1], tetraVerts[2], rho, color(1.0, 0.0, 0.0)));
	sceneList.push_back(triangle(tetraVerts[3], tetraVerts[0], tetraVerts[1], rho, color(1.0, 0.0, 0.0)));
	sceneList.push_back(triangle(tetraVerts[3], tetraVerts[0], tetraVerts[2], rho, color(1.0, 0.0, 0.0)));

	polyModel tetra(sceneList, glm::vec3(0.0f), TYPE, 1.0);
	return tetra;
}