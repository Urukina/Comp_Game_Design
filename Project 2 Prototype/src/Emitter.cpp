/* Nate Agpaoa - CS 134 Section 1
*
*/
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
	drawable = false;
	thrust = 0;
}

void Emitter::draw() {
	if (drawable) {
		TriangleShape::draw();
	}
	sys->draw();
}

void Emitter::update() {
	if (!started) return;


	float time = ofGetElapsedTimeMillis();
	if ((time - lastSpawned) > (1000.0 / rate)) {
		Sprite sprite;
		sprite.name = "Unnamed sprite";
		sprite.setScale(scale);
		sprite.lifespan = lifespan;
		sprite.setPos(getPos());
		sprite.angle = rotation;
		sprite.birthtime = time;
		sprite.thrust = thrust;
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


void Emitter::setRate(float r) {
	rate = r;
}

void Emitter::setSpriteScale(float sca) {
	scale = sca;
}

void Emitter::setSpriteRotation(float r) {
	rotation = r;
}




