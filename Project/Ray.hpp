#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Intersection.hpp"

struct CSGInfo
{
  double front_t;
  double back_t;
  glm::dvec4 front_N;
  glm::dvec4 back_N;
};

class Ray
{
public:
  glm::dvec4 origin;
  glm::dvec4 direction;
  double time;
  bool isMove;
  CSGInfo * csgInfo;

  Ray(glm::dvec4 origin, glm::dvec4 direction, double time);
  Ray(const Ray & ray);
  ~Ray();

};