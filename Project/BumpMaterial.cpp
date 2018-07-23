#include <iostream>
#include <lodepng/lodepng.h>

#include "BumpMaterial.hpp"

using namespace glm;
using namespace std;

#define COLOR_COEFFIENT 255.0

BumpMaterial::BumpMaterial () {}

BumpMaterial::BumpMaterial(std::string fileName) {
  bool check = lodepng::decode(image, width, height, fileName);
  if(!check) isValid = true;
}

BumpMaterial::~BumpMaterial () {}

double BumpMaterial::findValue(double u, double v){
  int iu = width * u;
  double delta_u = width * u - iu;
  int iv = height * v;
  double delta_v = height * v - iv;
  double f00 = getColor(iu + 1,iv + 1);
  double f10 = getColor(iu + 2,iv + 1);
  double f01 = getColor(iu + 1,iv + 2);
  double f11 = getColor(iu + 2,iv + 2);
  double fu0 = f00 + delta_u * (f10 - f00);
  double fu1 = f01 + delta_u * (f11 - f01);
  double value = fu0 + delta_v * (fu1 - fu0);
  return value;
}

double BumpMaterial::getColor(int i, int j){
  i = i - 1;
  j = j - 1;
  if(i >= width) i = width - 1;
  if(j >= height) j = height - 1;
  if(i < 0) i = 0;
  if(j < 0) j = 0;
  int point = (j * width + i) * 4;
  vec3 color = vec3(image[point] / COLOR_COEFFIENT,
                image[point + 1] / COLOR_COEFFIENT,
                image[point + 2] / COLOR_COEFFIENT);
  return (color.x + color.y + color.z) / 3.0;
}

glm::vec4 BumpMaterial::getNormal(double u, double v, vec4 N){
  if(!isValid) return N;
  double ew = 1 / (width * 1.0);
  double eh = 1 / (height * 1.0);
  float fu = (findValue(u + ew, v) - findValue(u - ew, v)) / (2 * ew);
  float fv = (findValue(u, v + eh) - findValue(u, v - eh)) / (2 * eh);
  vec3 pu = cross(vec3(N), vec3(0.0, 1.0, 0.0));
  vec3 pv = cross(vec3(N), vec3(1.0, 0.0, 0.0));
  vec3 D = fu * cross(vec3(N), pv) - fv * cross(vec3(N), pu);
  return normalize(N + vec4(D, 0));
}