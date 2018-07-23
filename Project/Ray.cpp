#include "Ray.hpp"

using namespace glm;

Ray::Ray(glm::dvec4 origin, glm::dvec4 direction, double time): origin(origin), direction(direction), time(time){
  isMove = false;
  csgInfo = NULL;
}

Ray::Ray(const Ray & ray): origin(ray.origin), direction(ray.direction), time(ray.time){
  isMove = false;
  csgInfo = NULL;
}

Ray::~Ray(){
  if(csgInfo) {
    delete csgInfo;
  }
}
