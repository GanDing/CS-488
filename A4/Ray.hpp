#pragma once

#include <glm/glm.hpp>

class Ray
{
public:
  glm::dvec4 origin;
  glm::dvec4 direction;

  Ray(glm::dvec4 origin, glm::dvec4 direction);
  Ray(const Ray & ray);
  ~Ray();
  
};