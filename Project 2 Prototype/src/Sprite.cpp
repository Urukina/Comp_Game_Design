/* Nate Agpaoa - CS 134 Section 1
* 
*/

#include "Sprite.h"

Sprite::Sprite() {
	verts.push_back(glm::vec3(-25, 25, 0));
	verts.push_back(glm::vec3(25, 25, 0));
	verts.push_back(glm::vec3(0, -25, 0));
	name = "Unnamed sprite";
	lifespan = -1;
	birthtime = 0;
	thrust = 0;
}

float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

void Sprite::draw() {
	if (drawable) {
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		image.loadImage("images/agent_sprite.png");
		image.draw(verts[0].x - image.getWidth() / 4.5 + 10, verts[0].y - image.getHeight());
		ofPopMatrix();
	}
	else {
		TriangleShape::draw();
	}
}

void SpriteSystem::add(Sprite s) {
	sprites.push_back(s);
}

void SpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}

void SpriteSystem::update() {
	if (sprites.size() == 0) return;
	vector<Sprite>::iterator it = sprites.begin();
	vector<Sprite>::iterator temp;
	while (it != sprites.end()) {
		if (it->lifespan != -1 && it->age() > it->lifespan) {
			temp = sprites.erase(it);
			it = temp;
		}
		else it++;
	}
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].force = sprites[i].heading * sprites[i].thrust;
		sprites[i].angularForce = sprites[i].angle;
		sprites[i].integrate();
	}
}

void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}
