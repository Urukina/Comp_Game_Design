#pragma once
#include "ofMain.h"
#include "Shape.h"

class TriangleShape : public Shape {

public:
	bool collide;
	TriangleShape() {}


	TriangleShape(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	void draw();
	bool inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	bool inside(const glm::vec3 p);
};

class Player : public TriangleShape {
public:
	ofImage img;
	int nEnergy;
	
	Player() {}

	Player(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	void draw();
	glm::vec3 heading();
	bool inside(const glm::vec3 p);
	void setEnergy(int e);
	bool outOfBounds(glm::vec3 p);

};