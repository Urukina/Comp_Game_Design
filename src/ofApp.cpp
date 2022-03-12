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
	gui.add(rate.setup("rate", 1, 0.5, 5));
	gui.add(life.setup("life", 5, 0, 10));
	gui.add(numAgents.setup("number of agents", 1, 1, 10));
	guiHide = true;

	// player setup
	p = Player(glm::vec3(-50, 50, 0), glm::vec3(50, 50, 0), glm::vec3(0, -50, 0));
	p.setPos(glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0));
	p.nEnergy = level;

	// agent setup
	numEmitters = 1;
	enemySpeed = 50;
	for (int i = 0; i < numEmitters; i++) {
		Emitter *agent = new Emitter(new SpriteSystem());
		randXPos = rand() % ofGetWindowWidth();
		randYPos = rand() % ofGetWindowHeight();
		agent->setPos(glm::vec3(randXPos,  randYPos , 0));
		agent->setSpeed(enemySpeed);
		agent->setSpriteScale(0.5);
		agent->setSpriteRotation(0);
		agent->drawable = false;
		multAgents.push_back(agent);
		agent->start();
	}

	offScreen = 0;
	background.loadImage("images/background_edited.png");

}


//--------------------------------------------------------------
void ofApp::update() {
	p.setSpeed(speed);
	p.setScale(scaleTo);
	p.setDrawn(image);
	for (Emitter* e: multAgents) {
		e->setRate(rate);
		e->setLifespan(life * 1000);
		e->setHeadingSprite(p.getPos());
		e->haveChildImg = aImage;
		//e->setPos(e->getPos() + 1 * glm::normalize(p.getPos() - e->getPos()));
		e->update();
	}
	if (numEmitters > numAgents) {
		removeAgent(numAgents);
	}
	else if (numEmitters < numAgents) {
		addAgent(numAgents);
	}
	checkCollisions();
	outOfBounds();
	// keep track of game time
	if (!gameOver) {
		elapsedTime = ofGetElapsedTimeMillis();
		if ((elapsedTime - lastTime) > 1000) {
			tSec++;
			// make the game challenging overtime by increasing the speed of the agents
			if (tSec % 30  == 0) {
				enemySpeed += 50;
				for (Emitter* e : multAgents) {
					Emitter* emitter = e;
					emitter->setSpeed(enemySpeed);
				}
			}
			if (tSec > 59) {
				tSec = 0;
				tMin++;
			}
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
		// reset the player position
		p.setPos(glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0));
		p.setRotation(0);
		p.setScale(1);
	}

}

//--------------------------------------------------------------
void ofApp::draw() {
	background.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	string nEnergyText, elapsedTime, fps;
	string totalTime;
	if (!gameStart) {
		ofDrawBitmapString("PRESS SPACE TO START", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
		ofDrawBitmapString("Controls:", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 10));
		ofDrawBitmapString("Up Arrow/Down Arrow: Move Up/Down", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 20));
		ofDrawBitmapString("Left Arrow/Right Arrow: Rotate Left/Right", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 30));
		ofDrawBitmapString("F4: Toggle Fullscreen", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 40));
	}
	else if (gameOver) {
		ofDrawBitmapString("GAME OVER", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
		ofDrawBitmapString("Press Spacebar to play again", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 60));
	}
	else {
		p.draw();
		for (Emitter* e : multAgents) {
			e->draw();
		}
		if (header) {
			ofDrawLine(p.getPos(), p.getPos() + 75 * p.heading());
		}
		// no. of lives remaining
		nEnergyText += "Lives: " + std::to_string(p.nEnergy) + "/" + std::to_string(level);
		ofDrawBitmapString(nEnergyText, ofPoint(ofGetWindowWidth()-90, 20));
		// display time
		elapsedTime += std::to_string(tMin) + ":" + std::to_string(tSec);
		ofDrawBitmapString(elapsedTime, ofPoint(ofGetWindowWidth()-40, 40));
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
					multAgents[i]->sys->remove(j);
					p.nEnergy--;
				}
			}
		}
	}
}

void ofApp::outOfBounds() {
	if (p.getPos().x < 0 || p.getPos().y < 0 || p.getPos().x > ofGetWindowWidth() || p.getPos().y > ofGetWindowHeight()) {
		p.setPos(glm::vec3(ofGetWindowWidth()/2, ofGetWindowHeight()/2,0));
		offScreen++;
		if (offScreen != 0 && offScreen % 3 == 0) {
			p.nEnergy--;
		}
	}
}

// note: adding more agents while having the agent sprite toggled will decreaese
// the fps of the game
void ofApp::addAgent(int n) {
	if (numEmitters < n) {
		cout << "called" << endl;
		int delta = n - numEmitters;
		numEmitters += delta;
		for (int i = 0; i < delta; i++) {
			cout << "Creating" << endl;
			Emitter* agent = new Emitter(new SpriteSystem());
			randXPos = rand() % ofGetWindowWidth();
			randYPos = rand() % ofGetWindowHeight();
			agent->setPos(glm::vec3(randXPos, randYPos, 0));
			agent->setSpeed(enemySpeed);
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
		cout << "called" << endl;
		numEmitters = numEmitters - n;
		for (int i = multAgents.size() - 1; i >= numEmitters; i--) {
			cout << "erasing" << endl;
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
	}
	if (keymap[OF_KEY_DOWN]) {
		p.setPos(p.getPos() - p.getSpeed() * p.heading());
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
	if (keymap['q']) {
	}
	if (keymap [' ']) {
		if (!gameStart) {
			for (Emitter* e : multAgents) {
				if (!e->started) {
					gameOver = false;
					e->start();
					p.nEnergy = level;
				}
			}
			gameStart = true;
		}
		else {
			enemySpeed = 50;
			for (Emitter* e : multAgents) {
				int newRandXPos = rand() % ofGetWindowWidth();
				int newRandYPos = rand() % ofGetWindowHeight();
				if (!e->started) {
					elapsedTime = ofGetElapsedTimeMillis();
					gameOver = false;
					e->setSpeed(enemySpeed);
					e->setPos(glm::vec3(newRandXPos, newRandYPos, 0));
					e->start();
					p.nEnergy = level;
					tSec = 0;
					tMin = 0;
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	keymap[key] = false;
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
