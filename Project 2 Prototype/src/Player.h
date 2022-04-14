/* Nate Agpaoa - CS 134 Section 1
*
*/
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
	void integrate();

	//physics data
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	glm::vec3 force = glm::vec3(0, 0, 0);
	float mass = 1.0;
	float damping = .99;
	float angularForce = 0;
	float angularVelocity = 0.0;
	float angularAcceleration = 0.0;
	bool bThrust = false;
	float prevDist = 0;
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

};