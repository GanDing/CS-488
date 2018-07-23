#include <iostream>
#include "Material.hpp"

Material::Material()
{
  m_bumpMaterial = NULL;
  m_kd = glm::vec3();
  m_ks = glm::vec3();
  m_coefficient = glm::vec3();
  m_shininess = 0.0;
  m_reflection = 0.0;
  m_refraction = 0.0;
}

Material::Material(const glm::vec3& kd, const glm::vec3& ks, const glm::vec3& coefficient, double shininess, double reflection, double refraction):
  m_kd(kd),
  m_ks(ks),
  m_coefficient(coefficient),
  m_shininess(shininess),
  m_reflection(reflection),
  m_refraction(refraction),
  m_bumpMaterial(NULL){}

glm::vec3 Material::getKd(double u, double v) {
  return m_kd;
}


glm::vec4 Material::getBumbMaterialN(double u, double v, glm::vec4 N) {
  if (m_bumpMaterial != NULL) {
    return m_bumpMaterial->getNormal(u, v, N);
  }
  return N;
}


void Material::setBumpMaterial(BumpMaterial * bumpMaterial){
  m_bumpMaterial = bumpMaterial;
}

Material::~Material()
{}
