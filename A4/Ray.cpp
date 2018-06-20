#include "Ray.hpp"

using namespace glm;

Ray::Ray(glm::dvec4 origin, glm::dvec4 direction): origin(origin), direction(direction){}

Ray::Ray(const Ray & ray): origin(ray.origin), direction(ray.direction){}

Ray::~Ray(){}
