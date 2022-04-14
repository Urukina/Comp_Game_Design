#pragma once
/* A modified version of the Emitter.h class that uses the triangle shape class.
*  The agents are represented as triangles as default.
*  Nate Agpaoa - CS 134 Section 1
*/

#include "Sprite.h"

class Emitter :public TriangleShape {
public:
	Emitter(SpriteSystem*);
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setSpriteScale(float);
	void setSpriteRotation(float);
	void setRate(float);
	void update();
	SpriteSystem* sys;
	float rate;
	float rotation;
	float scale;
	float lifespan;
	bool started;
	float lastSpawned;
	bool drawable;
	float thrust;
};
