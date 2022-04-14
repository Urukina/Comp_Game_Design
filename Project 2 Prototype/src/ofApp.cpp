/* Nate Agpaoa - CS 134 Section 1
*
*/
#include "ofApp.h"
#include <time.h>

//--------------------------------------------------------------
void ofApp::setup() {
	
	srand(time(nullptr));

	// gui setup
	ofSetVerticalSync(true);
	gui.setup();
	gui.add(speed.setup("speed", 1000, 100, 10000));
	gui.add(scaleTo.setup("scale", 1.0, 0.5, 2));
	gui.add(level.setup("energy level", 5, 1, 10));
	gui.add(header.setup("Draw heading", false));
	gui.add(image.setup("Draw player sprite", false));
	gui.add(aImage.setup("Draw agent sprite", false));
	gui.add(rate.setup("rate", 0.5, 0.5, 5));
	gui.add(life.setup("life", 5, 0, 30));
	gui.add(numAgents.setup("number of agents", 1, 1, 10));
	gui.add(thrust.setup("Enemy thrust", 200, 100, 1000));
	guiHide = true;

	// player setup
	p = Player(glm::vec3(-50, 50, 0), glm::vec3(50, 50, 0), glm::vec3(0, -50, 0));
	p.setPos(glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0));
	p.nEnergy = level;

	// agent setup
	numEmitters = 1;
	for (int i = 0; i < numEmitters; i++) {
		Emitter *agent = new Emitter(new SpriteSystem());
		randXPos = rand() % ofGetWindowWidth();
		randYPos = rand() % ofGetWindowHeight();
		agent->setPos(glm::vec3(randXPos,  randYPos , 0));
		agent->setSpriteScale(0.5);
		agent->setSpriteRotation(0);
		agent->drawable = false;
		agent->thrust = 0;
		multAgents.push_back(agent);
		agent->start();
	}
	prevHead = glm::vec3(0, 0, 0);
	
	//particle ray (player blaster) setup
	particleRay = ParticleEmitter(new ParticleSystem());
	particleRay.setOneShot(true);
	particleRay.setLifespan(2);
	particleRay.setParticleRadius(5);

	//particle explosion setup
	particleExplosion = ParticleEmitter(new ParticleSystem());
	particleExplosion.setOneShot(true);
	particleExplosion.setEmitterType(RadialEmitter);
	particleExplosion.setGroupSize(10);
	particleExplosion.setLifespan(2);
	particleExplosion.setVelocity(glm::vec3(-1000, 0, 0));
	particleExplosion.setParticleRadius(5);

	// sounds setup
	spriteExplosion.load("sounds/vine-boom.mp3");
	playerThrust.load("sounds/Rocket Thrusters-SoundBible.com-1432176431.mp3");
	collide.load("sounds/Metal-ding-sound-effect.mp3");

	background.loadImage("images/background_edited.png");

	isPlaySound = false;
}


//--------------------------------------------------------------
void ofApp::update() {
	//update player
	p.setSpeed(speed);
	p.setScale(scaleTo);
	p.setDrawn(image);

	//update agents
	for (Emitter* e: multAgents) {
		e->setRate(rate);
		e->setLifespan(life * 1000);
		//e->setHeadingSprite(p.getPos());
		for (int j = 0; j < e->sys->sprites.size(); j++) {
			e->sys->sprites[j].drawable = aImage;
			e->sys->sprites[j].heading =glm::normalize(p.getPos()- e->sys->sprites[j].getPos());
			float a = glm::orientedAngle(prevHead, e->sys->sprites[j].heading, glm::vec3(0, 0, 1));
			e->sys->sprites[j].angle = glm::degrees(a);
			prevHead = e->sys->sprites[j].heading;
		}
		e->thrust = thrust;
		e->update();
	}

	//update particle ray and particle explosion
	glm::mat4 M = p.getMatrix();
	playerTip = M * glm::vec4(p.verts[2], 1);
	particleRay.setVelocity(1000 * p.heading());
	particleRay.update();
	particleExplosion.update();

	//update number of agents based on agent slider
	if (numEmitters > numAgents) {
		removeAgent(numAgents);
	}
	else if (numEmitters < numAgents) {
		addAgent(numAgents);
	}

	//check for collisions betwen sprites and screen
	checkCollisions();
	checkParticleCollisions();
	outOfBounds();

	// keep track of game time
	if (!gameOver || gameStart) {
		playSound(isPlaySound);
		elapsedTime = ofGetElapsedTimeMillis();
		if ((elapsedTime - lastTime) > 1000) {
			if (!gameOver) {
				tSec++;
			}
			// make the game challenging overtime by increasing the speed of the agents
			lastTime = elapsedTime;
		}
		frameRate = ofGetFrameRate();
	}

	// it's possible to get negative lives
	// meaning that the existing sprites won't be removed once the game restarts
	// so I have to also check if the player's energy reaches below zero
	if (p.nEnergy <= 0) {
		p.nEnergy = 0;
		gameOver = true;
		for (Emitter* e : multAgents) {
			e->stop();
			// remove any exisiting sprites so that they won't continue to move when restarting
			for (int i = 0; i < e->sys->sprites.size(); i++) {
				e->sys->remove(i);
			}
		}
		for (int i = 0; i < particleExplosion.sys->particles.size(); i++) {
			particleExplosion.sys->remove(i);
		}
		// reset the player position
		p.setPos(glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0));
		p.setRotation(0);
		p.setScale(1);
	}

}

//--------------------------------------------------------------
void ofApp::draw() {
	
	// draw background
	background.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	string nEnergyText, elapsedTime, fps;
	string totalTime;

	//draw start screen and game over screen
	if (!gameStart) {
		ofDrawBitmapString("PRESS SPACE TO START", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
		ofDrawBitmapString("Controls:", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 10));
		ofDrawBitmapString("Up Arrow/Down Arrow: Move Up/Down", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 20));
		ofDrawBitmapString("Left Arrow/Right Arrow: Rotate Left/Right", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 30));
		ofDrawBitmapString("z: Fire", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 40));
		ofDrawBitmapString("F4: Toggle Fullscreen", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 50));
	}
	else if (gameOver) {
		int tSeconds = tSec;
		ofDrawBitmapString("GAME OVER", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
		elapsedTime += "Elapsed time: " + std::to_string(tSeconds);
		ofDrawBitmapString(elapsedTime, ofPoint(ofGetWindowWidth()/2, ofGetWindowHeight()/2 + 20));
		ofDrawBitmapString("Press Spacebar to play again", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 40));
	}
	else {
		// draw player
		p.draw();
		
		// draw agents
		for (Emitter* e : multAgents) {
			e->draw();
		}
		if (header) {
			ofDrawLine(p.getPos(), p.getPos() + (75*scaleTo) * p.heading());
		}

		// draw particle ray
		ofPushMatrix();
		ofTranslate(playerTip.x, playerTip.y);
		particleRay.draw();
		ofPopMatrix();

		// draw particle explosion
		ofPushMatrix();
		ofTranslate(partExX, partExY);
		particleExplosion.draw();
		ofPopMatrix();

		// draw hud
		// no. of lives remaining
		nEnergyText += "Lives: " + std::to_string(p.nEnergy) + "/" + std::to_string(level);
		ofDrawBitmapString(nEnergyText, ofPoint(ofGetWindowWidth()-90, 20));
		// display time
		elapsedTime += "Elapsed time: " + std::to_string(tSec);
		ofDrawBitmapString(elapsedTime, ofPoint(ofGetWindowWidth()-150, 40));
		// display fps
		fps += "FPS: " + std::to_string(frameRate);
		ofDrawBitmapString(fps, ofPoint(ofGetWindowWidth()-70, 60));
	}
	
	if (!guiHide) {
		gui.draw();
	}
	
}

void ofApp::checkCollisions() {
	for (int i = 0; i < multAgents.size(); i++) {
		for (int j = 0; j < multAgents[i]->sys->sprites.size(); j++) {
			glm::mat4 M = multAgents[i]->sys->sprites[j].getMatrix();
			for (int k = 0; k < multAgents[i]->sys->sprites[j].verts.size(); k++) {
				if (p.inside(M * glm::vec4(multAgents[i]->sys->sprites[j].verts[k], 1))) {
					//insert explosion here
					partExX = multAgents[i]->sys->sprites[j].getPos().x;
					partExY = multAgents[i]->sys->sprites[j].getPos().y;
					particleExplosion.start();
					spriteExplosion.play();
					if (!gameStart) {
						spriteExplosion.setVolume(0.0);
					}
					spriteExplosion.setVolume(0.5);
					collide.play();
					if (!gameStart) {
						collide.setVolume(0.0);
					}
					collide.setVolume(0.5);
					multAgents[i]->sys->remove(j);
					p.nEnergy--;
				}
			}
		}
	}
}

void ofApp::checkParticleCollisions() {
	for (int i = 0; i < multAgents.size(); i++) {
		for (int j = 0; j < multAgents[i]->sys->sprites.size(); j++) {
			Sprite s = multAgents[i]->sys->sprites[j];
			for (int k = 0; k < particleRay.sys->particles.size(); k++) {
				Particle pa = particleRay.sys->particles[k];
				if (glm::distance((glm::vec3)pa.position, s.getPos()) < pa.radius + s.getScale() * s.image.getWidth()/2) {
					partExX = multAgents[i]->sys->sprites[j].getPos().x;
					partExY = multAgents[i]->sys->sprites[j].getPos().y;
					particleExplosion.start();
					spriteExplosion.play();
					collide.play();
					multAgents[i]->sys->remove(j);
					particleRay.sys->remove(k);
					p.nEnergy++;
				}
			}
		}
	}
}


void ofApp::outOfBounds() {
	//updated the outOfBounds function so that the vertices of the player causes the player to teleport back to spawn
	//instead of having the player's current position touching the screen boundaries
	glm::mat4 M = p.getMatrix();
	glm::vec3 t = M * glm::vec4(p.verts[0], 1);
	glm::vec3 u = M * glm::vec4(p.verts[1], 1);
	glm::vec3 v = M * glm::vec4(p.verts[2], 1);
	bool insideVertOne = t.x < 0 || t.y < 0 || t.x > ofGetWindowWidth() || t.y > ofGetWindowHeight();
	bool insideVertTwo = u.x < 0 || u.y < 0 || u.x > ofGetWindowWidth() || u.y > ofGetWindowHeight();
	bool insideVertThree = v.x < 0 || v.y < 0 || v.x > ofGetWindowWidth() || v.y > ofGetWindowHeight();
	if (insideVertOne || insideVertTwo || insideVertThree) {
		collide.play();
		p.setPos(glm::vec3(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 0));
	}
}

void ofApp::playSound(bool sound) {
	if (!sound) {
		playerThrust.setLoop(true);
		playerThrust.play();
		playerThrust.setVolume(0.5);
	}
}

// note: adding more agents while having the agent sprite toggled will decreaese
// the fps of the game
void ofApp::addAgent(int n) {
	if (numEmitters < n) {
		int delta = n - numEmitters;
		numEmitters += delta;
		for (int i = 0; i < delta; i++) {
			Emitter* agent = new Emitter(new SpriteSystem());
			randXPos = rand() % ofGetWindowWidth();
			randYPos = rand() % ofGetWindowHeight();
			agent->setPos(glm::vec3(randXPos, randYPos, 0));
			agent->setSpriteScale(0.5);
			agent->setSpriteRotation(0);
			agent->drawable = false;
			multAgents.push_back(agent);
			agent->start();
		}
	}
}

void ofApp::removeAgent(int n) {
	if (numEmitters > n) {
		numEmitters = numEmitters - n;
		for (int i = multAgents.size() - 1; i >= numEmitters; i--) {
			multAgents.erase(multAgents.begin() + i);
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	keymap[key] = true;
	float rotate = p.getRotation();
	if (keymap[OF_KEY_RIGHT]) {
		p.setRotation(rotate + 10);
	}
	if (keymap[OF_KEY_LEFT]) {
		p.setRotation(rotate - 10);
	}
	if (keymap[OF_KEY_UP]) {
		p.setPos(p.getPos() + p.getSpeed() * p.heading());

		isPlaySound = true;
	}
	if (keymap[OF_KEY_DOWN]) {
		p.setPos(p.getPos() - p.getSpeed() * p.heading());
		isPlaySound = true;
	}
	if (keymap['h']) {
		guiHide = true;
	}
	if (keymap ['g']) {
		guiHide = false;
	}
	if (keymap[OF_KEY_F4]) {
		ofToggleFullscreen();
	}
	if (keymap['z']) {
		particleRay.start();
	}
	if (keymap [' ']) {
		if (!gameStart) {
			for (Emitter* e : multAgents) {
				if (!e->started) {
					gameOver = false;
					e->start();
				}
			}
			tSeconds = 0;
			tSec = 0;
			p.nEnergy = level;
			gameStart = true;
		}
		else {
			for (Emitter* e : multAgents) {
				int newRandXPos = rand() % ofGetWindowWidth();
				int newRandYPos = rand() % ofGetWindowHeight();
				if (!e->started) {
					gameOver = false;
					e->setPos(glm::vec3(newRandXPos, newRandYPos, 0));
					e->start();
				}
			}
			p.nEnergy = level;
			tSec = 0;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	keymap[key] = false;
	isPlaySound = false;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	glm::vec3 pressedMousePos = glm::vec3(x, y, 0);
	if (button == 0 && p.inside(pressedMousePos)) {
		p.setPressedPos(pressedMousePos);
		p.setPressed(true);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	glm::vec3 newMousePos = glm::vec3(x, y, 0);
	if (button == 0 && p.isPressed() == true) {
		p.setPos(p.getPos() + newMousePos - p.getPressedPos());
		p.setPressedPos(newMousePos);
		p.setPressed(false);
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
