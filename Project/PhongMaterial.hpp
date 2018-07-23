#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial();
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, const glm::vec3& coefficient, double shininess, double reflection, double refraction);
  virtual ~PhongMaterial();

  // glm::vec3 m_kd;
  // glm::vec3 m_ks;

  // double m_shininess;
  // double m_reflection;
};
