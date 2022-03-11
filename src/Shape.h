#pragma once
#include "ofMain.h"
class Shape {
private:
	glm::vec3 pos;
	float rotation = 0.0;
	float scale;
	glm::vec3 pressedPos;
	bool pressed;
	bool drawn;
	float speed;
public:
	vector<glm::vec3> verts;

	Shape() {
		pos = glm::vec3(0, 0, 0); 
		scale = 1;
	}
	virtual void draw() {}
	virtual bool inside(const glm::vec3 p) {
		return true;
	}

	glm::mat4 getMatrix();

	// practicing using getter and setter functions
	glm::vec3 getPos() const { return pos; }
	void setPos(glm::vec3 posi) { pos = posi; }
	float getRotation() const { return rotation; }
	void setRotation(float r) { rotation = r; }
	float getScale() const { return scale; }
	void setScale(float s) { scale = s; }

	glm::vec3 getPressedPos() const { return pressedPos; }
	void setPressedPos(glm::vec3 pp) { pressedPos = pp; }
	bool isPressed() const { return pressed; }
	void setPressed(bool p) { pressed = p; }
	bool isDrawn() const { return drawn; }
	void setDrawn(bool d) { drawn = d; }
	float getSpeed() const { return speed; }
	void setSpeed(float s) { speed = s / ofGetFrameRate(); }

};