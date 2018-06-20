#include "Intersection.hpp"

using namespace glm;


Intersection::Intersection() {
  phong_material = PhongMaterial();
  N = dvec4();
  t = -1.0;
}

Intersection::Intersection(PhongMaterial & phong_material, glm::dvec4 N, double t): 
  phong_material(phong_material), N(N), t(t){}

Intersection::~Intersection(){}

bool Intersection::is_intersect() {
  return t >= 0;
}