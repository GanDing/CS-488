#include "Intersection.hpp"

using namespace glm;


Intersection::Intersection() {
  inside = false;
  m_coefficient = vec3();
  texture_position = vec2();
  N = dvec4();
  t = -1.0;
  m_reflection = 0;
  ni = 0;
  nt = 0;
}

Intersection::Intersection(glm::dvec4 N, double t):
  N(N), t(t){
    inside = false;
    m_coefficient = vec3();
    texture_position = vec2();
    m_reflection = 0;
    ni = 0;
    nt = 0;
  }

Intersection::~Intersection(){}

bool Intersection::is_intersect() {
  return t >= 0;
}