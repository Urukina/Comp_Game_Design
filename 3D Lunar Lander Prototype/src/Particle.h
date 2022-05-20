#pragma once

// Edited by Nate Agpaoa - CS 134
#include "ofMain.h"

class ParticleForceField;
//edited to include angular forces
class Particle {
public:
	Particle();

	ofVec3f position;
	ofVec3f velocity;
	ofVec3f acceleration;
	ofVec3f forces;
	float angularVelocity;
	float angularAcceleration;
	float rotation;
	float angularForce;
	float	damping;
	float   mass;
	float   lifespan;
	float   radius;
	float   birthtime;
	void    integrate();
	void    draw();
	float   age();        // sec
	ofColor color;
};


