#pragma once
/* A modified version of the Emitter.h class that uses the triangle shape class.
*  The agents are represented as triangles as default.
*
*/

#include "Sprite.h"

class Emitter :public TriangleShape {
public:
	Emitter(SpriteSystem*);
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setSpeed(float);
	void setSpriteScale(float);
	void setSpriteRotation(float);
	void setRate(float);
	void setHeadingSprite(glm::vec3);
	float rotateSprite(glm::vec3 f, glm::vec3 g);
	void setChildImage(ofImage img, bool toggle);
	void update();
	SpriteSystem* sys;
	float rate;
	float speed;
	float rotation;
	float scale;
	glm::vec3 heading;
	glm::vec3 pHeading;
	glm::vec3 pHead;
	ofImage childImg;
	bool haveChildImg;
	float lifespan;
	bool started;
	float lastSpawned;
	bool drawable;
};
