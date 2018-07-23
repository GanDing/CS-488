#pragma once

#include <glm/glm.hpp>

class MotionBlur {
public:
  bool m_isMove;
  glm::vec4 m_posDirection;
  double m_posSpeed;


  MotionBlur();
  MotionBlur(bool isMove, glm::vec4 posDirection, double posSpeed);
  ~MotionBlur();
};