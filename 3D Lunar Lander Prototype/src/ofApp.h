#pragma once

// Nate Agpaoa - CS 134

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include <glm/gtx/intersect.hpp>
#include "Particle.h"
#include "ParticleEmitter.h";
#include "Octree.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void headingX();
		void headingZ();
		void loadVbo(ParticleEmitter *e, ofVbo *v);
		void renderParticleEmitter(ParticleEmitter* e, ofVbo *v, ofVec3f location);
		void collide();
		bool raySelectWithOctree(ofVec3f& pointRet);
		void initLightingAndMaterials();
		bool gameOver();
		glm::vec3 getMousePointOnPlane();
		void playThruster();
		
		ofEasyCam cam;
		ofCamera trackLanderCam, trackLanderCamTwo, landerPovCam, landerSidePovCam;
		ofCamera* theCam;
		ofxAssimpModelLoader lander, terrain;
		ofLight mainLight, keyLight, fillLight, rimLight;

		bool bWireframe;
		bool bDisplayPoints;
		bool bDisplayBBoxes;

		ofVec3f mouseDownPos;

		bool pointSelected, bLanderSelected;
		TreeNode selectedNode;
		ofVec3f point;


		Particle p;
		ParticleSystem s;
		ParticleEmitter explosion, landerThruster;


		GravityForce* gf;
		ImpulseRadialForce *irf;
		CylinderForce* cf;

		int radius = 5;

		ofVec3f dirX, dirZ, dirY;

		float thrust;

		float rotAngle;

		ofImage background;

		ofTexture  particleTex;

		ofVbo vboExplosion, vboThruster;
		ofShader shader;


		Octree octree;
		Vector3 initLanderPos, directionToSurface;
		Ray altitudeDetection;
		float altitudeAGL;

		bool toggleAltitude;
		vector<Box> bboxList;

		vector<Box> colBoxList;
		ofVec3f explosionLocation;

		int avgNormX, avgNormY, avgNormZ;
		bool boxIntersect;

		bool startSim;
		bool gameOverCrashLanding = false;
		bool victory = false;
		bool playThrustSound = false;
		ofVec3f landingZoneMin, landingZoneMax;
		Box landingZone;

		ofSoundPlayer landerThrustSound, explosionSound;

		int thrusterFuel, score;
		float time, elapsedTime, lastTime;
};
