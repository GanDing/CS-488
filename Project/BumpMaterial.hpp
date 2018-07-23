#pragma once

#include <glm/glm.hpp>
#include <vector>

class BumpMaterial {
public:
  BumpMaterial();
  BumpMaterial(std::string fileName);
  ~BumpMaterial();
  double findValue(double u, double v);
  double getColor(int i, int j);
  glm::vec4 getNormal(double u, double v, glm::vec4 N);

  std::vector<unsigned char> image;
  unsigned width;
  unsigned height;
  bool isValid = false;

};