#include "Sprite.h"

Sprite::Sprite() {
	verts.push_back(glm::vec3(-25, 25, 0));
	verts.push_back(glm::vec3(25, 25, 0));
	verts.push_back(glm::vec3(0, -25, 0));
	name = "Unnamed sprite";
	setSpeed(100);
	setScale(1);
	setRotation(0);
	lifespan = -1;
	birthtime = 0;
}

float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

void Sprite::setImage(ofImage img, bool toggle) {
	image = img;
	drawable = toggle;
}

void Sprite::draw() {
	ofSetColor(ofColor::goldenRod);
	ofFill();
	ofPushMatrix();
	ofMultMatrix(getMatrix());
	if (drawable) {
		image.loadImage("images/agent_sprite.png");
		image.draw(verts[0].x - image.getWidth() / 4.5 + 10, verts[0].y - image.getHeight());
		//cout << isDrawn() << endl;
	}
	else {
		ofDrawTriangle(verts[0], verts[1], verts[2]);
		//cout << isDrawn() << endl;
	}
	ofPopMatrix();
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
			// cout << "deleting sprite: " << it->name << endl;
			temp = sprites.erase(it);
			it = temp;
		}
		else it++;
	}
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].setPos(sprites[i].getPos() + sprites[i].getSpeed() * sprites[i].heading);
	}
}

void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}
