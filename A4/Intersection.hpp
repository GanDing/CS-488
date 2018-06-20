#pragma once

#include <glm/glm.hpp>

#include "PhongMaterial.hpp"


class Intersection
{
public:
  PhongMaterial phong_material;

  glm::dvec4 N;
  double t;

  Intersection();
  Intersection(PhongMaterial & phong_material, glm::dvec4 N, double t);
  ~Intersection();

  bool is_intersect();
  
};