#include <algorithm>
#include <iostream>

#include "Primitive.hpp"
#include "PhongMaterial.hpp"

using namespace glm;
using namespace std;

//---------Primitive-----------//
Primitive::Primitive(){}

Primitive::~Primitive(){}

Intersection Primitive::hit(Ray ray){
  return Intersection();
}

bool Primitive::ray_intersect_triangle(Ray ray, glm::dvec3 p0, glm::dvec3 p1, glm::dvec3 p2, double &t) {
  dvec3 edge1 = p1 - p0;
  dvec3 edge2 = p2 - p0;
  dvec3 edge3 = -dvec3(ray.direction.x, ray.direction.y, ray.direction.z);

  dvec4 normal = dvec4(cross(edge1, edge2), 0);
  if (dot(ray.direction, normal) > 0) {
    return false;
  }

  dvec3 R = dvec3(ray.origin.x - p0.x, ray.origin.y - p0.y, ray.origin.z - p0.z);
  double D = determinant(dmat3({edge1, edge2, edge3}));
  double D1 = determinant(dmat3({R, edge2, edge3}));
  double beta = D1/D;

  if (beta < 0) {
    return false;
  }
  double D2 = determinant(dmat3({edge1, R, edge3}));
  double gamma = D2 / D;
  if (gamma >= 0 && beta + gamma <= 1){
    double D3 = determinant(dmat3({edge1, edge2, R}));
    t = D3 / D;
    return true;
  }
  return false;

}



//---------NonhierSphere-----------//
NonhierSphere::NonhierSphere() {}

NonhierSphere::NonhierSphere(const glm::vec3& pos, double radius): m_pos(pos), m_radius(radius){
}

NonhierSphere::~NonhierSphere() {}

Intersection NonhierSphere::hit(Ray ray) {
  dvec4 center = dvec4(m_pos, 1);
  double a = dot(ray.direction, ray.direction);
  double b = 2 * dot(ray.direction, (ray.origin - center));
  double c = dot((ray.origin - center), (ray.origin - center)) - m_radius * m_radius;

  double root[2];
  size_t real_root = quadraticRoots(a, b, c, root);

  Intersection result = Intersection();
  if (real_root == 0) {
    return result;
  } else if(real_root == 1) {
    result.t = root[0];
  } else {
    result.t = std::min(root[0], root[1]);
  }

  dvec4 hit_point = ray.origin + result.t * ray.direction;
  result.N = normalize(hit_point - center);
  return result;
}


//---------Sphere-----------//
Sphere::Sphere(){
  nonhier_sphere = NonhierSphere(vec3(), 1.0);
}

Sphere::~Sphere(){}

Intersection Sphere::hit(Ray ray) {
  Intersection result;
  result = nonhier_sphere.hit(ray);
  return result;
}


//---------NonhierBox-----------//
NonhierBox::NonhierBox(){}

NonhierBox::NonhierBox(const glm::vec3& pos, double size) : NonhierBox(pos, dvec3(size, size, size)) {

}

NonhierBox::NonhierBox(const glm::vec3& pos, glm::dvec3 size)
    : m_pos(pos), m_size(size){

      double origin_x = pos.x;
      double origin_y = pos.y;
      double origin_z = pos.z;

      double size_x = size.x;
      double size_y = size.y;
      double size_z = size.z;

      // create vertices
      dvec3 vert_0 = dvec3(origin_x, origin_y, origin_z);
      dvec3 vert_1 = dvec3(origin_x + size_x, origin_y, origin_z);
      dvec3 vert_2 = dvec3(origin_x, origin_y + size_y, origin_z);
      dvec3 vert_3 = dvec3(origin_x, origin_y, origin_z + size_z);
      dvec3 vert_4 = dvec3(origin_x + size_x, origin_y + size_y, origin_z);
      dvec3 vert_5 = dvec3(origin_x + size_x, origin_y, origin_z + size_z);
      dvec3 vert_6 = dvec3(origin_x, origin_y + size_y, origin_z + size_z);
      dvec3 vert_7 = dvec3(origin_x + size_x, origin_y + size_y, origin_z + size_z);

      // create triangle
      // front
      triangle[0][0] = vert_7; triangle[0][1] = vert_6; triangle[0][2] = vert_3;
      triangle[1][0] = vert_3; triangle[1][1] = vert_5; triangle[1][2] = vert_7;
      // back
      triangle[2][0] = vert_0; triangle[2][1] = vert_2; triangle[2][2] = vert_4;
      triangle[3][0] = vert_4; triangle[3][1] = vert_1; triangle[3][2] = vert_0;
      // left
      triangle[4][0] = vert_0; triangle[4][1] = vert_3; triangle[4][2] = vert_6;
      triangle[5][0] = vert_6; triangle[5][1] = vert_2; triangle[5][2] = vert_0;
      // right
      triangle[6][0] = vert_4; triangle[6][1] = vert_7; triangle[6][2] = vert_5;
      triangle[7][0] = vert_5; triangle[7][1] = vert_1; triangle[7][2] = vert_4;
      // top
      triangle[8][0] = vert_2; triangle[8][1] = vert_6; triangle[8][2] = vert_7;
      triangle[9][0] = vert_7; triangle[9][1] = vert_4; triangle[9][2] = vert_2;
      // bottom
      triangle[10][0] = vert_0; triangle[10][1] = vert_1; triangle[10][2] = vert_5;
      triangle[11][0] = vert_5; triangle[11][1] = vert_3; triangle[11][2] = vert_0;
  }

NonhierBox::~NonhierBox(){}


Intersection NonhierBox::hit(Ray ray) {
  double t = -1.0;
  double baryPosition = 0.0;
  Intersection result = Intersection();
  for (int i = 0; i < 12; ++i) {
    bool is_hit = ray_intersect_triangle(ray, triangle[i][0], triangle[i][1], triangle[i][2], baryPosition);
    if (is_hit) {
      t = baryPosition;
      result.t = t;
      vec3 normal = normalize(cross(
        triangle[i][1] - triangle[i][0],
        triangle[i][2] - triangle[i][0]));
      dvec4 N = dvec4(normal, 0);
      result.N = N;
    }
  }
  if(!result.is_intersect()) {
    return Intersection();
  }
  return result;
  
}


//---------Cube-----------//
Cube::Cube(){
  nonhier_box = NonhierBox(vec3(), 1.0);
}

Cube::~Cube(){}

Intersection Cube::hit(Ray ray) {
  Intersection result;
  result = nonhier_box.hit(ray);
  return result;
}


