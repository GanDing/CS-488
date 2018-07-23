#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>

#include "Ray.hpp"
#include "Intersection.hpp"
#include "polyroots.hpp"
#include "MotionBlur.hpp"

class Primitive {
public:
  MotionBlur motionInfo;

  Primitive();
  virtual ~Primitive();
  virtual Intersection hit(Ray & ray);
  virtual glm::vec2 texturePosition(glm::vec4 & point, double time);
  bool ray_intersect_triangle(Ray ray, glm::dvec3 p0, glm::dvec3 p1, glm::dvec3 p2, double &t);
  bool ray_intersect_cylinder(Ray ray, double &t, glm::dvec4 &N);
  bool ray_intersect_cone(Ray ray, double &t, glm::dvec4 &N);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere();
  NonhierSphere(const glm::vec3& pos, double radius);
  virtual ~NonhierSphere();
  virtual Intersection hit(Ray & ray);
  virtual glm::vec2 texturePosition(glm::vec4 &point, double time);

private:
  glm::vec3 m_pos;
  double m_radius;
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
  virtual Intersection hit(Ray & ray);
  NonhierSphere nonhier_sphere;
};

class NonhierBox : public Primitive {
public:
  NonhierBox();
  NonhierBox(const glm::vec3& pos, double size);
  NonhierBox(const glm::vec3& pos, glm::dvec3 size);
  virtual ~NonhierBox();
  virtual Intersection hit(Ray & ray);
  virtual glm::vec2 texturePosition(glm::vec4 & point, double time);

private:
  glm::vec3 m_pos;
  glm::dvec3 m_size;
  glm::dvec3 triangle[12][3];
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();
  virtual Intersection hit(Ray & ray);
  NonhierBox nonhier_box;
};

class Cylinder: public Primitive {
public:
  Cylinder();
  virtual ~Cylinder();
  virtual Intersection hit(Ray & ray);
};

class Cone: public Primitive {
public:
  Cone();
  virtual ~Cone();
  virtual Intersection hit(Ray & ray);
};




