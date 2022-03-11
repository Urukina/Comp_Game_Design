#include "ofMain.h"
#include "Emitter.h"



Emitter::Emitter(SpriteSystem* spriteSys) {
	verts.push_back(glm::vec3(-25, 25, 0));
	verts.push_back(glm::vec3(25, 25, 0));
	verts.push_back(glm::vec3(0, -25, 0));
	sys = spriteSys;
	setScale(1);
	setRotation(0);
	lifespan = 3000;
	started = false;
	lastSpawned = 0;
	rate = 1;
	setSpeed(100);
	drawable = false;

}

void Emitter::draw() {

	if (drawable) {
		ofSetColor(ofColor::aqua);
		ofFill();
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		ofDrawTriangle(verts[0], verts[1], verts[2]);
		ofPopMatrix();
	}
	sys->draw();
}

void Emitter::update() {
	if (!started) return;

	float time = ofGetElapsedTimeMillis();
	if ((time - lastSpawned) > (1000.0 / rate)) {
		Sprite sprite;
		sprite.name = "Unnamed sprite";
		sprite.setSpeed(speed);
		sprite.setScale(scale);
		sprite.lifespan = lifespan;
		sprite.setPos(getPos());
		sprite.heading = heading;
		
		//cout << sprite.getRotation() + deg << endl;
		sprite.birthtime = time;
		sprite.pHead = pHead;
		if (haveChildImg) {
			sprite.setImage(childImg, haveChildImg);
		}
		sys->add(sprite);
		lastSpawned = time;
	}
	sys->update();
}

void Emitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

void Emitter::stop() {
	started = false;
}

void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setSpeed(float s) {
	speed = s;
}

void Emitter::setRate(float r) {
	rate = r;
}

void Emitter::setSpriteScale(float sca) {
	scale = sca;
}

void Emitter::setSpriteRotation(float r) {
	rotation = r;
}

void Emitter::setHeadingSprite(glm::vec3 hs) {
	heading = glm::normalize(hs - getPos());
}

float Emitter::rotateSprite(glm::vec3 f, glm::vec3 g) {
	float rot = glm::orientedAngle(f, g, glm::vec3(0, 0, 1));
	return glm::degrees(rot);
}

void Emitter::setChildImage(ofImage img, bool toggle) {
	childImg = img;
	haveChildImg = toggle;
}

