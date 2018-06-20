#include "PhongMaterial.hpp"


PhongMaterial::PhongMaterial(){
  m_kd = glm::vec3();
  m_ks = glm::vec3();
  m_shininess = 0.0;
}

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess){}

PhongMaterial::~PhongMaterial()
{}
