#include "PhongMaterial.hpp"


PhongMaterial::PhongMaterial(): Material(){}

PhongMaterial::PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, const glm::vec3& coefficient, double shininess, double reflection, double refraction)
	: Material(kd, ks, coefficient, shininess, reflection, refraction){}

PhongMaterial::~PhongMaterial()
{}
