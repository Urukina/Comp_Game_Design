#pragma once
/* A modified version of the Sprite.h class that uses the triangle shape class.
*  The agents are represented as triangles as default.
*
*/
#include "Player.h"
class Sprite : public TriangleShape {
public:
	Sprite();
	void draw();
	float age();
	void setImage(ofImage img, bool toggle);
	string name;
	glm::vec3 heading;
	glm::vec3 pHead;
	ofImage image;
	bool  drawable;
	float speed;
	float birthtime;
	float lifespan;
};

class SpriteSystem {
public:
	void add(Sprite);
	void remove(int);
	void update();
	void draw();
	vector<Sprite> sprites;
};
