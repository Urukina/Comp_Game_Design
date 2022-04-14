#pragma once
/* A modified version of the Sprite.h class that uses the triangle shape class.
*  The agents are represented as triangles as default.
*  Nate Agpaoa - CS 134 Section 1
*/
#include "Player.h"
class Sprite : public TriangleShape {
public:
	Sprite();
	void draw();
	float age();
	string name;
	glm::vec3 heading;
	glm::vec3 pHead;
	ofImage image;
	bool  drawable;
	float birthtime;
	float lifespan;
	float angle;
	float thrust;
};

class SpriteSystem {
public:
	void add(Sprite);
	void remove(int);
	void update();
	void draw();
	vector<Sprite> sprites;
};
