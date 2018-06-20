#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>

#include "Ray.hpp"
#include "Intersection.hpp"
#include "polyroots.hpp"

class Primitive {
public:
  Primitive();
  virtual ~Primitive();
  virtual Intersection hit(Ray ray);
  bool ray_intersect_triangle(Ray ray, glm::dvec3 p0, glm::dvec3 p1, glm::dvec3 p2, double &t);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere();
  NonhierSphere(const glm::vec3& pos, double radius);
  virtual ~NonhierSphere();
  virtual Intersection hit(Ray ray);

private:
  glm::vec3 m_pos;
  double m_radius;
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
  virtual Intersection hit(Ray ray);
  NonhierSphere nonhier_sphere;
};

class NonhierBox : public Primitive {
public:
  NonhierBox();
  NonhierBox(const glm::vec3& pos, double size);
  NonhierBox(const glm::vec3& pos, glm::dvec3 size);
  virtual ~NonhierBox();
  virtual Intersection hit(Ray ray);

private:
  glm::vec3 m_pos;
  glm::dvec3 m_size;
  glm::dvec3 triangle[12][3];
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();
  virtual Intersection hit(Ray ray);
  NonhierBox nonhier_box;
};




