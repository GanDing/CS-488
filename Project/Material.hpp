#pragma once

#include <glm/glm.hpp>

#include "BumpMaterial.hpp"

class Material {
public:
  BumpMaterial * m_bumpMaterial;
  glm::vec3 m_kd;
  glm::vec3 m_ks;
  glm::vec3 m_coefficient;

  double m_shininess;
  double m_reflection;
  double m_refraction;

  void setBumpMaterial(BumpMaterial * bumpMaterial);
  virtual glm::vec3 getKd(double u, double v);
  virtual glm::vec4 getBumbMaterialN(double u, double v, glm::vec4 N);
  virtual ~Material();

protected:
  Material();
  Material(const glm::vec3& kd, const glm::vec3& ks, const glm::vec3& m_coefficient, double shininess, double reflection, double refraction);
};
