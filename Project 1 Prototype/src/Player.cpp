#include "ofMain.h"
#include "Player.h"

/*Nate Agpaoa - CS 134 Section 1
* 
*/

TriangleShape::TriangleShape(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
}

void TriangleShape::draw() {
	ofPushMatrix();
	ofMultMatrix(getMatrix());
	ofDrawTriangle(verts[0], verts[1], verts[2]);
	ofPopMatrix();
}

bool TriangleShape::inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	glm::vec3 v1 = glm::normalize(p1 - p);
	glm::vec3 v2 = glm::normalize(p2 - p);
	glm::vec3 v3 = glm::normalize(p3 - p);
	float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
	float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
	float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, 1));
	if (a1 < 0 && a2 < 0 && a3 < 0) return true;
	else return false;
}

bool TriangleShape::inside(const glm::vec3 p) {
	glm::vec3 p0 = glm::inverse(getMatrix()) * glm::vec4(p, 1);
	return inside(p0, verts[0], verts[1], verts[2]);
}

Player::Player(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
}

void Player::draw() {
	if (isDrawn()) {
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		img.loadImage("images/neso_wish.png");
		img.draw(verts[0].x - img.getWidth() / 4.5f + 20, verts[0].y - img.getHeight());
		ofPopMatrix();
	}
	else {
		TriangleShape::draw();
	}
	
}

bool Player::inside(const glm::vec3 p) {
	if (!isDrawn()) {
		return TriangleShape::inside(p);
	}
	glm::vec3 sprite = glm::inverse(getMatrix()) * glm::vec4(p, 1);
	int width = img.getWidth();
	int height = img.getHeight();
	if (sprite.x > -width / 2 && sprite.x < width / 2 && sprite.y > -height / 2 && sprite.y < height / 2) {
		int x = sprite.x + width / 2;
		int y = sprite.y + height / 2;
		ofColor color = img.getColor(x, y);
		return (color.a != 0);
	}
	else return false;
}

glm::vec3 Player::heading() {
	float angle = glm::radians(getRotation());
	return glm::normalize(glm::vec3(glm::sin(angle), -glm::cos(angle), 0.0));
}
