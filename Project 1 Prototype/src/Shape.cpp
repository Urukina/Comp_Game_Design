#include "ofMain.h"
#include "Shape.h"

glm::mat4 Shape::getMatrix() {
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(pos));
	glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
	glm::mat4 sca = glm::scale(glm::mat4(1.0), glm::vec3(1.0, 1.0, 1.0) * scale);
	return (trans * rot * sca);
}