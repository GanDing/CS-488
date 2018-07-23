#include <lodepng/lodepng.h>
#include <iostream>
#include <string>

#include "TextureMaterial.hpp"

using namespace glm;
using namespace std;

#define COLOR_COEFFIENT 255.0

TextureMaterial::TextureMaterial(string fileName, const glm::vec3& ks, const glm::vec3& coefficient, double shininess, double reflection, double refraction)
  :Material(vec3(), ks, coefficient, shininess, reflection, refraction)
{
  bool check = lodepng::decode(image, width, height, fileName);
}

TextureMaterial::~TextureMaterial(){}

glm::vec3 TextureMaterial::getColor(int i, int j){
  if(i == width) {
    i = i - 1;
  }
  if(j == height) {
    j = j - 1;
  }
  int point = (j * width + i) * 4;
  if(point > image.size()) {
    cout << "pt:" << i << " " << j << " " << point << "  size: " << image.size() << endl;
  }
  vec3 color = vec3(image[point] / COLOR_COEFFIENT,
                    image[point + 1] / COLOR_COEFFIENT,
                    image[point + 2] / COLOR_COEFFIENT);
  return color;
}

glm::vec3 TextureMaterial::getKd(double u, double v){
  double di = (width - 1) * u;
  double dj = (height - 1) * v;
  // round
  int i = int(di);
  int j = int(dj);
  double up = di - i;
  double vp = dj - j;

  vec3 c[2][2];
  vec3 kd = vec3();
  for(int a = 0; a < 2; a++){
    for(int b = 0; b < 2; b++){
      c[a][b] = getColor(i + a,j + b);
      kd += (float)(((1 - a) * (1 - up) + a * up) *
                    ((1 - b) * (1 - vp) + b * vp)) *
                    c[a][b];
    }
  }
  return kd;
}

