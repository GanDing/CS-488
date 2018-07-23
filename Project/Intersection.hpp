#pragma once

#include <glm/glm.hpp>

//#include "PhongMaterial.hpp"


class Intersection
{
public:
  glm::vec3 m_kd;
  glm::vec3 m_ks;
  glm::vec3 m_coefficient;
  glm::dvec4 N;

  glm::vec2 texture_position;

  bool inside;

  double t;
  double m_shininess;
  double m_reflection;
  double ni;
  double nt;

  Intersection();
  Intersection(glm::dvec4 N, double t);
  ~Intersection();

  bool is_intersect();

};