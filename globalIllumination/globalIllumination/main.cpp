#include <iostream>
#include "definition.h"
#include "camera.h"
#include "polyModel.h"
#include "implicitModel.h"

#include <chrono>


polyModel createScene()
{
	vertex roofVertices[6];
	vertex floorVertices[6];
	vertex lightVerts[3];
	std::vector<triangle> sceneList;
	double wallRho = 0.8;
	double lightWallRho = 0.3;
	double maxRho = 1.0;
	roofVertices[0] = vertex(0.0f, 6.0f, 5.0f, 1.0f);
	roofVertices[1] = vertex(-3.0f, 0.0f, 5.0f, 1.0f);
	roofVertices[2] = vertex(0.0f, -6.0f, 5.0f, 1.0f);
	roofVertices[3] = vertex(10.0f, 6.0f, 5.0f, 1.0f);
	roofVertices[4] = vertex(13.0f, 0.0f, 5.0f, 1.0f);
	roofVertices[5] = vertex(10.0f, -6.0f, 5.0f, 1.0f);

	floorVertices[0] = vertex(0.0f, 6.0f, -5.0f, 1.0f);
	floorVertices[1] = vertex(-3.0f, 0.0f, -5.0f, 1.0f);
	floorVertices[2] = vertex(0.0f, -6.0f, -5.0f, 1.0f);
	floorVertices[3] = vertex(10.0f, 6.0f, -5.0f, 1.0f);
	floorVertices[4] = vertex(13.0f, 0.0f, -5.0f, 1.0f);
	floorVertices[5] = vertex(10.0f, -6.0f, -5.0f, 1.0f);
	//roof triangles
	lightVerts[0] = vertex(5.5f, 0.0f, 0.0f, 1.0f);
	lightVerts[1] = vertex(4.5f, 1.0f, 0.0f, 1.0f);
	lightVerts[2] = vertex(4.5f, -1.0f, 0.0f, 1.0f);
	//light source
	sceneList.push_back(triangle(lightVerts[1], lightVerts[0], lightVerts[2], 1.0, color(1.0, 1.0, 1.0), true));

	sceneList.push_back(triangle(roofVertices[0], roofVertices[2], roofVertices[1], maxRho,color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[0], roofVertices[5], roofVertices[2], maxRho, color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[0], roofVertices[3], roofVertices[5], maxRho, color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[3], roofVertices[4], roofVertices[5], maxRho, color(1.0, 1.0, 1.0)));
	//floor triangles
	sceneList.push_back(triangle(floorVertices[0], floorVertices[1], floorVertices[2], maxRho, color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(floorVertices[0], floorVertices[2], floorVertices[5], maxRho, color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(floorVertices[0], floorVertices[5], floorVertices[3], maxRho, color(1.0, 1.0, 1.0)));
	sceneList.push_back(triangle(floorVertices[3], floorVertices[5], floorVertices[4], maxRho, color(1.0, 1.0, 1.0)));
	//wall 1, red
	sceneList.push_back(triangle(roofVertices[1], floorVertices[2], floorVertices[1], lightWallRho, color(1.0, 0.0, 0.0)));
	sceneList.push_back(triangle(floorVertices[2], roofVertices[1], roofVertices[2], lightWallRho, color(1.0, 0.0, 0.0)));
	//wall 2, blue
	sceneList.push_back(triangle(floorVertices[5], floorVertices[2], roofVertices[5], wallRho,  color(0.0, 0.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[2], roofVertices[5], floorVertices[2], wallRho,  color(0.0, 0.0, 1.0)));
	//wall 3, green
	sceneList.push_back(triangle(floorVertices[5], roofVertices[5], roofVertices[4], wallRho, color(0.0, 1.0, 0.0)));
	sceneList.push_back(triangle(roofVertices[4], floorVertices[4], floorVertices[5], wallRho, color(0.0, 1.0, 0.0)));
	//wall 4
	sceneList.push_back(triangle(roofVertices[4], floorVertices[3], floorVertices[4], wallRho,  color(1.0, 1.0, 0.0)));
	sceneList.push_back(triangle(roofVertices[3], floorVertices[3], roofVertices[4], wallRho,  color(1.0, 1.0, 0.0)));
	//wall 5
	sceneList.push_back(triangle(roofVertices[0], floorVertices[0], floorVertices[3], wallRho,  color(1.0, 0.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[0], floorVertices[3], roofVertices[3], wallRho, color(1.0, 0.0, 1.0)));
	//wall 6
	sceneList.push_back(triangle(roofVertices[0], floorVertices[1], floorVertices[0], lightWallRho, color(0.0, 1.0, 1.0)));
	sceneList.push_back(triangle(roofVertices[1], floorVertices[1], roofVertices[0], lightWallRho, color(0.0, 1.0, 1.0)));

	polyModel scene(sceneList, glm::vec3(0.0, 0.0, 0.0), DIFFUSE, 1.0);
	return scene;
}
polyModel createTetra()
{
	vertex tetraVerts[4];
	std::vector<triangle> sceneList;
	double tetraRho = 1.0;
	tetraVerts[0] = vertex(5.5f, -4.0f, -4.9f, 1.0f);
	tetraVerts[1] = vertex(7.0f, -4.0f, -4.9f, 1.0f);
	tetraVerts[2] = vertex(7.0f, -4.0f, -2.5f, 1.0f);
	tetraVerts[3] = vertex(7.0f, -1.0f, -4.9f, 1.0f);
	/*
	tetraVerts[0] = vertex(7.5f, -4.0f, -4.9f, 1.0f);
	tetraVerts[1] = vertex(9.0f, -4.0f, -4.9f, 1.0f);
	tetraVerts[2] = vertex(9.0f, -4.0f, -2.5f, 1.0f);
	tetraVerts[3] = vertex(9.0f, -1.0f, -4.9f, 1.0f);
	*/

	sceneList.push_back(triangle(tetraVerts[0], tetraVerts[2], tetraVerts[1], tetraRho, color(1.0, 0.0, 0.0)));
	sceneList.push_back(triangle(tetraVerts[3], tetraVerts[1], tetraVerts[2], tetraRho, color(1.0, 0.0, 0.0)));
	sceneList.push_back(triangle(tetraVerts[3], tetraVerts[0], tetraVerts[1], tetraRho, color(1.0, 0.0, 0.0)));
	sceneList.push_back(triangle(tetraVerts[3], tetraVerts[0], tetraVerts[2], tetraRho, color(1.0, 0.0, 0.0)));
	//sceneList.push_back(triangle(tetraVerts[3], tetraVerts[0], tetraVerts[2], color(1.0, 0.5, 0.5),true));
	
	polyModel tetra(sceneList, glm::vec3(0.0f), DIFFUSE, 1.0);
	return tetra;
}

int main(int, char*[])
{
	std::vector<object*> objectList;

	polyModel scene = createScene();
	polyModel tetra = createTetra();
	implicitModel sphere(1.5f, glm::vec3(6.0f, 2.0f, -3.4f), MIRROR, 1.0);

	objectList.push_back(&scene);
	objectList.push_back(&sphere);
	objectList.push_back(&tetra);

	camera *cam = new camera(objectList);


	//CONFIG
	cam->setRenderingMode(MULTI_THREAD);
	cam->setBranchFactor(2);
	cam->setShadowRays(2);
	cam->setDepth(1);
	cam->setInitRay(4);
	


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