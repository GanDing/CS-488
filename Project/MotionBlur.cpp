#include <iostream>
#include "MotionBlur.hpp"

using namespace glm;
using namespace std;

MotionBlur::MotionBlur(){
  m_isMove = false;
  m_posDirection = vec4();
  m_posSpeed = 0;
}

MotionBlur::MotionBlur(bool isMove, glm::vec4 posDirection, double posSpeed):
  m_isMove(isMove),
  m_posDirection(posDirection),
  m_posSpeed(posSpeed){}

MotionBlur::~MotionBlur(){}
