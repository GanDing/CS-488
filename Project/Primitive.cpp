#include <algorithm>
#include <float.h>
#include <iostream>

#include "Primitive.hpp"
#include "PhongMaterial.hpp"
#include "MotionBlur.hpp"

using namespace glm;
using namespace std;

//---------Primitive-----------//
Primitive::Primitive(){}

Primitive::~Primitive(){}

Intersection Primitive::hit(Ray & ray){
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
  double beta = D1 / D;

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

bool Primitive::ray_intersect_cylinder(Ray ray, double &t, glm::dvec4 &N) {
  double a = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z;
  double b = 2 * (ray.origin.x * ray.direction.x + ray.origin.z * ray.direction.z);
  double c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z - 1;

  double root[2];

  size_t real_root = quadraticRoots(a, b, c, root);

  if (real_root == 0) {
    return false;
  } else if (real_root == 1) {
    t = root[0];
    double y = ray.origin.y + ray.direction.y * t;
    if (y <= 1 && y >= -1) {
      t = root[0];
      dvec4 hit_point = ray.origin + ray.direction * t;
      N = normalize(dvec4(hit_point.x, 0, hit_point.z, 0));
      return true;
    }
  } else if (real_root == 2) {
    // If both are > 1, miss
    // if both are < -1, miss
    // If one is > 1, one is < 1, hit the top cap and side,
    // if one is < -1, one is > -1, hit the bottom cap, and side

    double t_0 = root[0];
    double t_1 = root[1];

    // let z_0 is smaller than z_1
    if (t_0 > t_1) {
      std::swap(t_0, t_1);
    }

    double p_0 = ray.origin.y + ray.direction.y * t_0;
    double p_1 = ray.origin.y + ray.direction.y * t_1;

    if (p_0 < -1 && p_1 >= -1) {
      // hit the bottom circle
      double th = t_0 + (t_1 - t_0) * (p_0 + 1) / (p_0 - p_1);

      if (th > 0) {
        t = th;
        N = dvec4(0, -1, 0, 0);
        return true;
      }
    } else if (p_0 >= -1 && p_0 <= 1 && t_0 > 0) {
      // Hit the cylinder curve.
      t = t_0;
      dvec4 hit_point = ray.origin + ray.direction * t;
      N = normalize(dvec4(hit_point.x, 0, hit_point.z, 0));
      return true;
    } else if (p_0 > 1 && p_1 <= 1) {
      // hit the top circle
      double th = t_0 + (t_1 - t_0) * (p_0 - 1) / (p_0 - p_1);
      if (th > 0) {
        t = th;
        N = dvec4(0, 1, 0, 0);
        return true;
      }
    }
  }
  return false;
}

bool Primitive::ray_intersect_cone(Ray ray, double &t, glm::dvec4 &N) {

  double a = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z - ray.direction.y * ray.direction.y;
  double b = 2 * (ray.origin.x * ray.direction.x + ray.origin.z * ray.direction.z - ray.origin.y * ray.direction.y);
  double c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z - ray.origin.y * ray.origin.y;

  double root[2];
  size_t real_root = quadraticRoots(a, b, c, root);

  if (real_root == 0) {
    // No roots, miss.
    return false;
  } else if (real_root == 1) {
    t = root[0];
    double y = ray.origin.y + ray.direction.y * t;
    if (y <= 1 && y >= 0) {
      glm::dvec4 hit_point = ray.origin + ray.direction * t;
      double x2_z2_sqrt = sqrt(hit_point.x * hit_point.x + hit_point.z * hit_point.z);
      dvec3 p_x {1.0, hit_point.x / x2_z2_sqrt, 0.0};
      dvec3 p_z {0.0, hit_point.z / x2_z2_sqrt, 1.0};
      dvec3 normal = glm::cross(p_x, p_z);
      N = dvec4(normalize(normal), 0);
    }
  } else if (real_root == 2) {
    double t_0 = root[0];
    double t_1 = root[1];

    if (t_0 > t_1) {
      // Make sure z1 is smaller than z2.
      std::swap(t_0, t_1);
    }

    glm::dvec4 p_0 = ray.origin + ray.direction * t_0;
    glm::dvec4 p_1 = ray.origin + ray.direction * t_1;

    if (p_0.y < 0 && p_1.y >= 0 && p_1.y <= 1 && t_1 > 0) {

      t = t_1;

      dvec4 hit_point = ray.origin + ray.direction * t;
      double x2_z2_sqrt = sqrt( hit_point.x * hit_point.x + hit_point.z * hit_point.z );
      dvec3 p_x {1.0, hit_point.x / x2_z2_sqrt, 0.0};
      dvec3 p_z {0.0, hit_point.z / x2_z2_sqrt, 1.0};
      N = dvec4(normalize(cross(p_x, p_z)), 0);
      return true;

    } else if (p_0.y >= 0 && p_0.y <= 1 && t_0 > 0) {
      t = t_0;

      dvec4 hit_point = ray.origin + ray.direction * t;
      double x2_z2_sqrt = sqrt( hit_point.x * hit_point.x + hit_point.z * hit_point.z );
      dvec3 p_x {1.0, hit_point.x / x2_z2_sqrt, 0.0};
      dvec3 p_z {0.0, hit_point.z / x2_z2_sqrt, 1.0};
      N = dvec4(normalize(cross(p_x, p_z)), 0);
      return true;
    } else if (p_0.y > 1 && p_1.y >= 0 && p_1.y <=  1 && t_1 > 0) {
            // Hit the top cap
      double th = ( 1 - ray.origin.y ) / ray.direction.y;
      dvec4 hit_point = ray.origin + ray.direction * th;

      if (th >= 0 && (hit_point.x <= 1 && hit_point.x >= -1) && (hit_point.z <= 1 && hit_point.z >= -1)) {
        t = th;
        N = glm::dvec4(0, 1, 0, 0);
        return true;
      }
    }
  }
    return false;
}

glm::vec2 Primitive::texturePosition(vec4 & point, double time) {
  return vec2();
}


//---------NonhierSphere-----------//
NonhierSphere::NonhierSphere() {}

NonhierSphere::NonhierSphere(const glm::vec3& pos, double radius): m_pos(pos), m_radius(radius){
}

NonhierSphere::~NonhierSphere() {}

Intersection NonhierSphere::hit(Ray & ray) {
  if(motionInfo.m_isMove){
    ray.isMove = true;
  }
  dvec4 center = dvec4(m_pos, 1) + (motionInfo.m_posSpeed * ray.time) * dvec4(motionInfo.m_posDirection);
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
    if (root[0] > root[1]) {
      std::swap(root[0], root[1]);
    }
    result.t = root[0];
  }
  dvec4 hit_point = ray.origin + result.t * ray.direction;
  result.N = normalize(hit_point - center);
  if (root[0] < 0 && root[1] > 0) {
    result.N = -result.N;
  }
  vec4 point = vec4(ray.origin + result.t * ray.direction);
  result.texture_position = texturePosition(point, ray.time);
  if(real_root == 2 && root[0] > 0) {
    CSGInfo * csgPoint = new CSGInfo();
    csgPoint->front_t = root[0];
    csgPoint->back_t = root[1];
    csgPoint->front_N = result.N;
    dvec4 hit_point_2 = ray.origin + root[1] * ray.direction;
    dvec4 N_2 = normalize(hit_point_2 - center);
    csgPoint->back_N = N_2;
    ray.csgInfo = csgPoint;
  }
  return result;
}

vec2 NonhierSphere::texturePosition(vec4 & point, double time){
  vec4 curPos = vec4(m_pos, 1.0f) + (float)(motionInfo.m_posSpeed * time) * motionInfo.m_posDirection;
  double rz = sqrt(pow(point.x - curPos.x, 2) + pow(point.z - curPos.z, 2));
  double thetah;
  double thetav;
  if(m_radius == 0) return vec2();
  double deltax = point.x - curPos.x;
  double deltay = point.y - curPos.y;
  double deltaz = point.z - curPos.z;
  if(point.z >= curPos.z){
    thetah = atan(deltax / deltaz) + M_PI / 2.0;
  } else {
    thetah = 3 * M_PI / 2.0 + atan(deltax / deltaz);
  }

  double val = glm::min(rz / m_radius, 1.0);
  val = glm::max(val, 0.0);
  if(point.y >= curPos.y){
    thetav = asin(val);
  } else {
    thetav = M_PI - asin(val);
  }
  vec2 result = vec2(thetah / (2 * M_PI), thetav / M_PI);
  result = vec2(glm::min(result.x, 1.0f), glm::min(result.y, 1.0f));
  result = vec2(glm::max(result.x, 0.0f), glm::max(result.y, 0.0f));
  return result;
}


//---------Sphere-----------//
Sphere::Sphere(){
  nonhier_sphere = NonhierSphere(vec3(), 1.0);
}

Sphere::~Sphere(){}

Intersection Sphere::hit(Ray & ray) {
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


Intersection NonhierBox::hit(Ray & ray) {
  double t = -1.0;
  double baryPosition = 0.0;
  Intersection result = Intersection();
  if(motionInfo.m_isMove){
    ray.isMove = true;
  }
  for (int i = 0; i < 12; ++i) {
    //dvec3 a =
    vec4 a = vec4(triangle[i][0], 1) + (float)(motionInfo.m_posSpeed * ray.time) * motionInfo.m_posDirection;
    vec4 b = vec4(triangle[i][1], 1) + (float)(motionInfo.m_posSpeed * ray.time) * motionInfo.m_posDirection;
    vec4 c = vec4(triangle[i][2], 1) + (float)(motionInfo.m_posSpeed * ray.time) * motionInfo.m_posDirection;
    bool is_hit = ray_intersect_triangle(ray, vec3(a), vec3(b), vec3(c), baryPosition);
    if (is_hit) {
      t = baryPosition;
      result.t = t;
      vec3 normal = normalize(cross(
        triangle[i][1] - triangle[i][0],
        triangle[i][2] - triangle[i][0]));
      dvec4 N = dvec4(normal, 0);
      result.N = N;
      vec4 point = ray.origin + t * ray.direction;
      result.texture_position = texturePosition(point, ray.time);
    }
  }
  if(!result.is_intersect()) {
    return Intersection();
  }
  return result;
}

glm::vec2 NonhierBox::texturePosition(vec4 & point, double time) {

  vec4 curPos = vec4(m_pos, 1.0f) + (float)(motionInfo.m_posSpeed * time) * motionInfo.m_posDirection;
  vec3 new_point = vec3(point - curPos);
  double size = m_size.x;
  double ggepsilon = 0.01;
  vec2 result;
  if(abs(new_point.x - 0) < ggepsilon){
    result = vec2(new_point.z / size, (size-new_point.y) / size);
  } else if(abs(new_point.x - size) < ggepsilon){
    result = vec2((size-new_point.z) / size, (size-new_point.y) / size);
  } else if(abs(new_point.y - 0) < ggepsilon){
    result = vec2(new_point.x / size,new_point.z / size);
  } else if(abs(new_point.y - size) < ggepsilon){
    result = vec2((size-new_point.x) / size, (size-new_point.z) / size);
  } else if(abs(new_point.z - 0) < ggepsilon){
    result = vec2((size-new_point.x) / size, (size-new_point.y) / size);
  } else if(abs(new_point.z - size) < ggepsilon){
    result = vec2(new_point.x / size, (size-new_point.y) / size);
  } else {
    result = vec2();
  }
  result = vec2(glm::min(result[0], 1.0f), glm::min(result[1], 1.0f));
  result = vec2(glm::max(result[0], 0.0f), glm::max(result[1], 0.0f));
  return result;
}


//---------Cube-----------//
Cube::Cube(){
  nonhier_box = NonhierBox(vec3(), 1.0);
}

Cube::~Cube(){}

Intersection Cube::hit(Ray & ray) {
  Intersection result;
  result = nonhier_box.hit(ray);
  return result;
}


//---------Cylinder-----------//
Cylinder::Cylinder() {}



Cylinder::~Cylinder() {}

Intersection Cylinder::hit(Ray & ray) {

  double t = -1.0;
  dvec4 N = dvec4();
  Intersection result = Intersection();
  bool is_hit = ray_intersect_cylinder(ray, t, N);
  if (is_hit) {
    result.t = t;
    result.N = N;
  }

  if (result.is_intersect()) {
    // Test if it's inside the cylinder.
    double x = ray.origin.x;
    double y = ray.origin.y;
    double z = ray.origin.z;

    if (std::abs(y) <= 1 && (x * x + z * z <= 1)) {
      // inside.
      result.N = -result.N;
    }
  }
  return result;
}

//---------Cone-----------//
Cone::Cone() {
}

Cone::~Cone() {}

Intersection Cone::hit(Ray & ray) {
  double t = -1.0;
  dvec4 N = dvec4();
  Intersection result = Intersection();
  bool is_hit = ray_intersect_cone(ray, t, N);
  if (is_hit) {
    result.t = t;
    result.N = N;
  }

  return result;
}
