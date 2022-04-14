/* Nate Agpaoa - CS 134 Section 1
*
*/
#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Player.h"
#include "Emitter.h"
#include "Particle.h"
#include "ParticleEmitter.h"


class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();
	void checkCollisions();
	void checkParticleCollisions();
	void outOfBounds();
	void addAgent(int n);
	void removeAgent(int n);
	void playSound(bool sound);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	Player p;

	vector<Emitter*> multAgents;
	ParticleEmitter particleRay, particleExplosion;
	int numEmitters;
	ofImage sp;

	glm::vec3 playerTip;

	glm::vec3 prevHead;

	int partExX, partExY;

	ofSoundPlayer playerThrust, collide, spriteExplosion;

	bool isPlaySound;

	ofImage background;

	int randXPos, randYPos;

	ofxFloatSlider speed;
	ofxFloatSlider scaleTo;
	ofxIntSlider level;
	ofxFloatSlider rate, life;
	ofxIntSlider numAgents;
	ofxVec3Slider velocity;
	ofxToggle header;
	ofxToggle image, aImage;
	ofxFloatSlider thrust;
	ofxPanel gui;

	map<int, bool> keymap;

	bool guiHide;

	bool gameOver = false,	gameStart = false;

	float elapsedTime, lastTime;
	int tSec, frameRate;
	float tSeconds;
};

