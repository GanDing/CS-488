#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Material.hpp"

class TextureMaterial : public Material {
public:
  TextureMaterial(std::string fileName, const glm::vec3& ks, const glm::vec3& coefficient, double shininess, double reflection, double refraction);
  ~TextureMaterial();
  glm::vec3 getColor(int i, int j);
  virtual glm::vec3 getKd(double u, double v);

  std::vector<unsigned char> image;
  unsigned width;
  unsigned height;
};