

#include "ovr.hpp"

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif


#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#ifndef _PLAYER_H_
#define _PLAYER_H_
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <unordered_map>
#include "Cube.h"
#include "objectdata.hpp"
#include "Model.h"


class Player {
public:
  Player();


  ~Player();

  Model* head;
  Model* leftHand;
  Model* rightHand;
  Model* origin;


  glm::mat4 toWorld;
  glm::mat4 headPosition;
  glm::mat4 headOrientation;
  std::unordered_map<int,int> headColorMap;
  glm::mat4 leftControllerPosition;
  glm::mat4 rightControllerPosition;
  glm::mat4 leftControllerOrientation;
  glm::mat4 rightControllerOrientation;
  bool leftHandVisible, rightHandVisible;


  void draw(GLuint shaderProgram, const glm::mat4 &projection, const glm::mat4 &view, const bool isSelf);

  void Player::updateState(PlayerData p);

};


#endif
