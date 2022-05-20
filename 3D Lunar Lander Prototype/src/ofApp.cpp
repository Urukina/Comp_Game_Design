#include "ofApp.h"

//Nate Agpaoa - CS 134

//--------------------------------------------------------------
void ofApp::setup(){
	startSim = true;

	explosionSound.load("sounds/vine-boom.mp3");
	landerThrustSound.load("sounds/Rocket Thrusters-SoundBible.com-1432176431.mp3");
	
	time = 120.00;
	score = 200;
	// camera setup
	cam.setDistance(10);
	cam.setNearClip(0.1);
	cam.setFov(65.5);
	theCam = &cam;
	trackLanderCam.setGlobalPosition(glm::vec3(20, 20, 20));
	trackLanderCamTwo.setGlobalPosition(glm::vec3(-10, 10, -10));
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofSetBackgroundColor(ofColor::black);

	ofEnableSmoothing();
	ofEnableDepthTest();
	ofEnableLighting();

	// lighting setup
	initLightingAndMaterials();

	// shader setup
	ofDisableArbTex();

	if (!ofLoadImage(particleTex, "images/dot.png")) {
		cout << "Particle Texture File: images/dot.png not found" << endl;
		ofExit();
	}
	


	shader.load("shaders/shader");

	// background setup
	if (!background.loadImage("images/stars_background.png")) {
		cout << "Image File: images/stars_background not found" << endl;
		ofExit();
	}

	// lander and terrain setup
	lander.loadModel("geo/lander.obj");
	lander.setScaleNormalization(false);
	lander.setPosition(0, 0, 0);
	for (int i = 0; i < lander.getMeshCount(); i++) {
		bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
	}

	terrain.loadModel("geo/moon-houdini.obj");
	terrain.setScaleNormalization(false);

	octree.create(terrain.getMesh(0), 20);

	// particle setup. used for movement of the lander via physics
	// the moon's gravity is approximately 1.625 m/s^2
	// source: https://usm.maine.edu/planet/does-moon-have-gravity
	gf = new GravityForce(ofVec3f(0, -1.625f, 0));

	p.position = glm::vec3(0, 200, 0);
	p.lifespan = 1000000;
	s.add(p);
	s.addForce(new TurbulenceForce(glm::vec3(-1.625f, 0, -1.625f), glm::vec3(1.625f, 0, 1.625f)));
	s.addForce(gf);
	
	// parameters setup for particle effects
	irf = new ImpulseRadialForce(1000);
	cf = new CylinderForce(500);
	irf->setHeight(0.2);
	explosion.sys->addForce(irf);

	//setup for explosion effect
	explosion.setVelocity(ofVec3f(0, 0, 0));
	explosion.setOneShot(true);
	explosion.setEmitterType(RadialEmitter);
	explosion.setGroupSize(10000);
	explosion.setRandomLife(true);
	explosion.setLifespanRange(ofVec2f(1, 6));
	explosion.setParticleRadius(radius);

	//setup for thrust effect
	landerThruster.sys->addForce(cf);
	landerThruster.setVelocity(ofVec3f(0, 0, 0));
	landerThruster.setOneShot(true);
	landerThruster.setEmitterType(SphereEmitter);
	landerThruster.setGroupSize(250);
	landerThruster.setRandomLife(true);
	landerThruster.setLifespanRange(ofVec2f(0.1, 0.2));
	landerThruster.setParticleRadius(radius);

	//thrust direction for the lander
	 
	dirY = ofVec3f(0, 1, 0);
	
	rotAngle = 0;

	toggleAltitude = true;

	landingZoneMin = ofVec3f(40, -1, 40);
	landingZoneMax = ofVec3f(60, 1, 60);
	landingZone = Box(Vector3(landingZoneMin.x, landingZoneMin.y, landingZoneMin.x), 
		Vector3(landingZoneMax.x, landingZoneMax.y, landingZoneMax.z));
}

// code for ligting setup used from lab 8
void ofApp::initLightingAndMaterials() {
	keyLight.setup();
	keyLight.enable();
	keyLight.setAreaLight(1, 1);
	keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	keyLight.setSpecularColor(ofFloatColor(1, 1, 1));

	keyLight.rotate(45, ofVec3f(0, 1, 0));
	keyLight.rotate(-45, ofVec3f(1, 0, 0));
	keyLight.setPosition(0, 100, 5);

	fillLight.setup();
	fillLight.enable();
	fillLight.setSpotlight();
	fillLight.setScale(.05);
	fillLight.setSpotlightCutOff(15);
	fillLight.setAttenuation(2, .001, .001);
	fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	fillLight.setDiffuseColor(ofFloatColor(1, 0, 1));
	fillLight.setSpecularColor(ofFloatColor(1, 1, 1));
	fillLight.rotate(-10, ofVec3f(1, 0, 0));
	fillLight.rotate(-45, ofVec3f(0, 1, 0));
	fillLight.setPosition(-150, 50, 5);

	rimLight.setup();
	rimLight.enable();
	rimLight.setSpotlight();
	rimLight.setScale(.05);
	rimLight.setSpotlightCutOff(30);
	rimLight.setAttenuation(.2, .001, .001);
	rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	rimLight.setDiffuseColor(ofFloatColor(0.34, 1, 0.5));
	rimLight.setSpecularColor(ofFloatColor(1, 0.23, 0));
	rimLight.rotate(180, ofVec3f(0, 1, 0));
	rimLight.setPosition(0, 50, -150);
}

// code used from the shaders particle emitter example
// added parameters for loading the vertex buffer for more than one emitter
// passed by reference because passing by value crashes program
void ofApp::loadVbo(ParticleEmitter *e, ofVbo *v) {
	if (e->sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < e->sys->particles.size(); i++) {
		points.push_back(e->sys->particles[i].position);
		sizes.push_back(ofVec3f(radius));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	v->clear();
	v->setVertexData(&points[0], total, GL_STATIC_DRAW);
	v->setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}

// collision detection and response for landing the lander
void ofApp::collide() {
	if (s.particles[0].velocity.y > 0) return; //ignore collision if it's moving up
	if (!startSim) return; //ignore collision on debug mode
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	colBoxList.clear();
	boxIntersect = octree.intersect(bounds, octree.root, colBoxList);
	if (boxIntersect) {
		// gets the colliding location of the lander for the explosion to render
		explosionLocation = lander.getPosition();
		// set the normal to be parallel to the surface (i.e. xz-plane)
		// just a placeholder for the norm since I can't figure out the colliding boxes' surface normal
		ofVec3f norm = ofVec3f(0, 1, 0);
		norm.normalize();
		// hard coded the restitution because I'm lazy to create a gui slider for that
		ofVec3f f = (0.85 + 1.0) * ((-s.particles[0].velocity.dot(norm) * norm));
		// check is the lander does not crash or has landed on the LZ
		// otherwise it has not landed on the LZ
		if (f.y > 4.5) {
			explosionSound.play();
			explosion.sys->reset();
			explosion.start();
			gameOverCrashLanding = true;
		}
		else if (landingZone.overlap(bounds)) {
			victory = true;
		}
		else gameOverCrashLanding = true;
		s.particles[0].forces += ofGetFrameRate() * f;
	}
}

// ray selection used for altitude detection
// code taken from project 3
bool ofApp::raySelectWithOctree(ofVec3f& pointRet) {
	pointSelected = octree.intersect(altitudeDetection, octree.root, selectedNode);
	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	return pointSelected;
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSeedRandom();
	headingX();
	headingZ();
	collide();
	
	gf->set(ofVec3f(ofRandom(-1.625f, 1.625f), -1.625f, ofRandom(-1.625f, 1.625f)));
	if (startSim) {
		s.update();
		explosion.update();
		landerThruster.update();
	}
	playThruster();
	// the lander's position and rotation is based off the the attached particle's position
	lander.setPosition(s.particles[0].position.x, s.particles[0].position.y, s.particles[0].position.z);
	lander.setRotation(1, s.particles[0].rotation, 0, 1, 0);

	// create ray from the lander that points down from its current position to the ground 
	initLanderPos = Vector3(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z);
	directionToSurface = Vector3(0, -lander.getPosition().y, 0);
	directionToSurface.normalize();
	altitudeDetection = Ray(initLanderPos, directionToSurface);

	// track lander cam follows the lander's position
	// lander pov cam gets the top view of the lander
	// using the top view cam makes it easier to control the lander in my opinion
	trackLanderCam.lookAt(glm::vec3(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z));
	trackLanderCamTwo.lookAt(glm::vec3(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z));
	landerPovCam.setGlobalPosition(glm::vec3(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z));
	landerPovCam.lookAt(glm::vec3(lander.getPosition().x, 0, lander.getPosition().z));

	// use the ray select to find the nearest point
	// get the lander's y position and subtract with the selected point's y position
	raySelectWithOctree(point);
	if (pointSelected) {
		altitudeAGL = lander.getPosition().y - point.y;
	}

	if (time < 0) time = 0;
}

//--------------------------------------------------------------
void ofApp::draw(){
	loadVbo(&explosion, &vboExplosion);
	loadVbo(&landerThruster, &vboThruster);
	// I don't know why, but drawing the backgroud will cover everything else (the moon terrain, the lander, etc.)
	// background.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	// ofSetColor(ofColor::white);
	// 
	// Render the models using the cpu
	theCam->begin();
	ofPushMatrix();
	ofEnableLighting();
	terrain.drawFaces();
	lander.drawFaces();
	// draw the LZ
	ofNoFill();
	ofSetColor(ofColor::aliceBlue);
	Octree::drawBox(landingZone);

	ofNoFill();
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	// draw for debugging purposes
	if (!startSim) {
		Octree::drawBox(bounds);
		keyLight.draw();
		fillLight.draw();
		rimLight.draw();
	}
	ofPopMatrix();
	theCam->end();

	glDepthMask(GL_FALSE);
	
	
	// Render the particle effects using the gpu
	renderParticleEmitter(&explosion, &vboExplosion, explosionLocation);
	renderParticleEmitter(&landerThruster, &vboThruster, lander.getPosition());

	// set back the depth mask
	//
	glDepthMask(GL_TRUE);

	// hud 
	string str, str2, str3, str4, str5, str6, str7, str8, str9;
	str += "Altitude (AGL): " + std::to_string(altitudeAGL);
	ofSetColor(ofColor::white);
	if (toggleAltitude) ofDrawBitmapString(str, 0, 40);
	str2 += "Fuel (seconds): " + std::to_string(time);
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str2, 0, 20);

	str9 += "FPS: " + std::to_string(ofGetFrameRate());
	ofDrawBitmapString(str9, ofGetWindowWidth() - 120, 20);

	// game over screens
	str3 += "You lose: crash landing or landed off LZ!";
	str6 += "Press space to start again";
	if (gameOverCrashLanding) {
		ofDrawBitmapString(str3, ofGetWindowWidth() / 2 - 20, ofGetWindowHeight() / 2);
		ofDrawBitmapString(str6, ofGetWindowWidth() / 2 - 20, ofGetWindowHeight() / 2 + 20);
	}
	str4 += "You win! Score: " + std::to_string(score + time);
	str8 += "Nice landing!";
	if (victory) {
		ofDrawBitmapString(str4, ofGetWindowWidth() / 2 - 20, ofGetWindowHeight() / 2);
		ofDrawBitmapString(str8, ofGetWindowWidth() / 2 - 20, ofGetWindowHeight() / 2 + 20);
		ofDrawBitmapString(str6, ofGetWindowWidth() / 2 - 20, ofGetWindowHeight() / 2 + 40);
	}
	ofDrawBitmapString("UP/DOWN/LEFT/RIGHT: Move forward/back/left/right", ofGetWindowWidth() - 390, 40);
	ofDrawBitmapString("W/S: Move up/down", ofGetWindowWidth()-160, 60);
	ofDrawBitmapString("A/D: Rotate left/right", ofGetWindowWidth() - 180, 80);
	ofDrawBitmapString("F2/F3/F4: Switch Cameras", ofGetWindowWidth() - 200, 100);
	ofDrawBitmapString("F1: Switch to easy cam", ofGetWindowWidth() - 200, 120);
	ofDrawBitmapString("e: toggle altitude", ofGetWindowWidth() - 160, 140);
	ofDrawBitmapString("c: toggle mouse input for easy cam", ofGetWindowWidth() - 280, 160);
	ofDrawBitmapString("z: toggle debug mode", ofGetWindowWidth() - 160, 180);
	ofDrawBitmapString("f: toggle fullscreen", ofGetWindowWidth() - 160, 200);

	
}
// code also used from the shaders particle emitter example
// added parameters for rendering more than one emitter
// passed by reference because passing by value crashes program
// position passed as a paramter to draw at a specific location
void ofApp::renderParticleEmitter(ParticleEmitter* e, ofVbo* v, ofVec3f location) {
	ofSetColor(255, 100, 90);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();
	// begin drawing in the camera
	//
	shader.begin();
	theCam->begin();
	glPushMatrix();
	glTranslated(location.x, location.y, location.z);
	// this makes everything look glowy :)
	//
	
	// draw particle emitter here..
	//
	particleTex.bind();
	v->draw(GL_POINTS, 0, (int)e->sys->particles.size());
	particleTex.unbind();

	//  end drawing in the camera
	// 
	glPopMatrix();
	theCam->end();
	shader.end();

	ofDisablePointSprites();
	ofDisableBlendMode();
	ofEnableAlphaBlending();
}

bool ofApp::gameOver() {
	return gameOverCrashLanding || victory;
}

void ofApp::playThruster() {
	if (!playThrustSound)
	{
		landerThrustSound.setLoop(true);
		landerThrustSound.play();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	// methods use to move the lander was taken from the midterm
	switch (key) {
	case OF_KEY_UP:
		if (gameOver()) break;
		if (time <= 0) break;
		if (!startSim) break;
		landerThruster.sys->reset();
		landerThruster.start();
		s.particles[0].forces += 50 * dirX;
		playThrustSound = true;
		time-=0.1;
		break;
	case OF_KEY_DOWN:
		if (gameOver()) break;
		if (time <= 0) break;
		if (!startSim) break;
		landerThruster.sys->reset();
		landerThruster.start();
		s.particles[0].forces -= 50 * dirX;
		playThrustSound = true;
		time-=0.1;
		break;
	case OF_KEY_LEFT:
		if (gameOver()) break;
		if (time <= 0) break;
		if (!startSim) break;
		landerThruster.sys->reset();
		landerThruster.start();
		s.particles[0].forces -= 50 * dirZ;
		playThrustSound = true;
		time-=0.1;
		break;
	case OF_KEY_RIGHT:
		if (gameOver()) break;
		if (time <= 0) break;
		if (!startSim) break;
		landerThruster.sys->reset();
		landerThruster.start();
		s.particles[0].forces += 50 * dirZ;
		playThrustSound = true;
		time-=0.1;
		break;
	case 'a':
		if (gameOver()) break;
		if (time <= 0) break;
		if (!startSim) break;
		landerThruster.sys->reset();
		landerThruster.start();
		s.particles[0].angularForce--;
		playThrustSound = true;
		time-=0.1;
		break;
	case 'd':
		if (gameOver()) break;
		if (time <= 0) break;
		if (!startSim) break;
		landerThruster.sys->reset();
		landerThruster.start();
		s.particles[0].angularForce++;
		playThrustSound = true;
		time-=0.1;
		break;
	case 'w':
		if (gameOver()) break;
		if (time <= 0) break;
		if (!startSim) break;
		landerThruster.sys->reset();
		landerThruster.start();
		s.particles[0].forces += 50 * glm::vec3(0, 1, 0);
		playThrustSound = true;
		time-=0.1;
		break;
	case 's':
		if (gameOver()) break;
		if (time <= 0) break;
		if (!startSim) break;
		landerThruster.sys->reset();
		landerThruster.start();
		s.particles[0].forces -= 50 * glm::vec3(0, 1, 0);
		playThrustSound = true;
		time-=0.1;
		break;
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case ' ':
		// reset parameters
		if (gameOver() || lander.getPosition().y < 0) {
			gameOverCrashLanding = false;
			victory = false;
			s.particles[0].position = ofVec3f(ofRandom(-100, 100), 200, ofRandom(-100, 100));
			time = 120.00;
		}
		break;
	case'e':
		toggleAltitude = !toggleAltitude;
		break;
	case'z':
		startSim = !startSim;
		break;
	case 't':
		cam.setPosition(lander.getPosition());
		break;
	case 'r':
		cam.reset();
		break;
	case OF_KEY_F1:
		theCam = &cam;
		break;
	case OF_KEY_F2:
		theCam = &trackLanderCam;
		break;
	case OF_KEY_F3:
		theCam = &trackLanderCamTwo;
		break;
	case OF_KEY_F4:
		theCam = &landerPovCam;
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch (key) {
	case OF_KEY_UP:
		playThrustSound = false;
		break;
	case OF_KEY_DOWN:
		playThrustSound = false;
		break;
	case OF_KEY_LEFT:
		playThrustSound = false;
		break;
	case OF_KEY_RIGHT:
		playThrustSound = false;
		break;
	case 'w':
		playThrustSound = false;
		break;
	case 's':
		playThrustSound = false;
		break;
	case 'a':
		// reset the angular force so it doesn't keep rotating forever
		s.particles[0].angularForce = 0;
		playThrustSound = false;
		break;
	case 'd':
		s.particles[0].angularForce = 0;
		playThrustSound = false;
		break;
	case 'c':
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	glm::vec3 mouse = getMousePointOnPlane();

	if (!bLanderSelected) return;

	glm::vec3 delta = mouse - mouseDownPos;


	glm::vec3 landerPos = lander.getPosition();
	landerPos += delta;

	lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
	s.particles[0].position = lander.getPosition();
	mouseDownPos = mouse;

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;
	if (startSim) return;

	//  implement you code here to select the rover
	//  if Selected, draw box in a different color
	glm::vec3 p = getMousePointOnPlane();
	glm::vec3 origin = theCam->getPosition();
	Vector3 v = Vector3(p.x, p.y, p.z);
	Vector3 w = Vector3(origin.x, origin.y, origin.z);
	Vector3 u = v - w;
	u.normalize();
	Ray r = Ray(w, u);

	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	if (bounds.intersect(r, -1000, 1000)) {
		bLanderSelected = true;
		mouseDownPos = p;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bLanderSelected = false;
}

glm::vec3 ofApp::getMousePointOnPlane() {
	// Setup our rays
	//
	glm::vec3 origin = theCam->getPosition();
	glm::vec3 camAxis = theCam->getZAxis();
	glm::vec3 mouseWorld = theCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

// creates the heading vector for the x value on the xz-plane since
// the lander is being rotated on the y-axis.
void ofApp::headingX() {
	rotAngle = s.particles[0].rotation;
	dirX = glm::normalize(glm::vec3(sin(rotAngle), 0, -cos(rotAngle)));
}

// The z heading is perpendicular to the x heading.
void ofApp::headingZ() {
	dirZ = glm::normalize(glm::vec3(-dirX.z, 0, dirX.x));
}
