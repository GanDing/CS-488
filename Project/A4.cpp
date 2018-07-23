#include <glm/ext.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <atomic>
#include <limits>

#include "A4.hpp"

#define DISTANCE 10.0
#define EPSILON 0.01

// ray
#define MAX_HIT 3

// reflection
#define REFLECTION false

// refraction
#define REFRACTION false

// anti-alising
#define ADATIVE_REGULAR_SAMPLING false
#define REGULAR_SAMPLING_MAX 4
#define RED_THRESHOLDS  0.4
#define BLUE_THRESHOLDS  0.6
#define GREEN_THRESHOLDS  0.3
#define REGULAR_SAMPLING_SIZE 3

// multi thread
#define THREAD_NUM 1

// soft shadow
#define AREA_LIGHT false
#define AREA_LIGHT_LOOP 15
#define LIGHT_WIDTH 50.0
#define LIGHT_HEIGHT 50.0

// motion blur
#define START_TIME -1
#define FINAL_TIME 0

// DOF
#define FIELD false
#define FIELD_LOOP 10
#define DOF 0

using namespace glm;
using namespace std;

SceneNode * root_node;
vec3 ambient_light;
dmat4 scene_to_world_trans;
list<Light *> light_beam;
vec3 eye_vec;
size_t nx;
size_t ny;

bool test = true;

void A4_Render(
    // What to render
    SceneNode * root,

    // Image to write to, set to a given width and height
    Image & image,

    // Viewing parameters
    const glm::vec3 & eye,
    const glm::vec3 & view,
    const glm::vec3 & up,
    double fovy,

    // Lighting parameters
    const glm::vec3 & ambient,
    const std::list<Light *> & lights
) {

  // Fill in raytracing code here...
  std::cout << "Calling A4_Render(\n" <<
      "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
      "\t" << "view: " << glm::to_string(view) << std::endl <<
      "\t" << "up:   " << glm::to_string(up) << std::endl <<
      "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
      "\t" << "lights{" << std::endl;

  for(const Light * light : lights) {
    std::cout << "\t\t" <<  *light << std::endl;
  }
  std::cout << "\t}" << std::endl;
  std:: cout <<")" << std::endl;

  nx = image.height();
  ny = image.width();

  double w = 2 * DISTANCE * tan(radians(fovy / 2));
  double h = 2 * DISTANCE * tan(radians(fovy / 2));

  dmat4 t1 = T1(nx, ny, DISTANCE);
  dmat4 s2 = S2(w, h, nx, ny);
  dmat4 r3 = R3(up, view);
  dmat4 t4 = T4(eye);

  scene_to_world_trans = t4 * r3 * s2 * t1;

  root_node = root;
  ambient_light = ambient;
  light_beam = lights;
  eye_vec = eye;

  for (uint y = 0; y < h; ++y) {
    for (uint x = 0; x < w; ++x) {
      // Red: increasing from top to bottom
      image(x, y, 0) = (double)y / h;
      // Green: increasing from left to right
      image(x, y, 1) = (double)x / w;
      // Blue: in lower-left and upper-right corners
      image(x, y, 2) = ((y < h/2 && x < w/2)
              || (y >= h/2 && x >= w/2)) ? 1.0 : 0.0;
    }
  }

  if (ADATIVE_REGULAR_SAMPLING) {
    cout << "super sampling " << endl;
  }

  auto renderPixel = [] (int x, int y) -> vec3 {
   vec3 finalColor;
   vec3 color;
   if (ADATIVE_REGULAR_SAMPLING) {
     finalColor = adativeRegularSampling(x, y);
   } else {
    dvec4 perturb = vec4();
    int loop = FIELD ? FIELD_LOOP : 1;
    for (int i = 0; i < loop; ++i) {
      perturb.x = (rand() % 100 * 0.01) * DOF;
      perturb.y = (rand() % 100 * 0.01) * DOF;
      perturb.z = (rand() % 100 * 0.01) * DOF;
      dvec4 world_point = scene_to_world_trans * dvec4(x, y, 0, 1);
      dvec4 rayOrigin = dvec4(eye_vec, 1) + perturb;
      dvec4 rayDirection = normalize(world_point - dvec4(eye_vec, 1));
      Ray ray = Ray(rayOrigin, rayDirection , START_TIME);
      while(ray.time < FINAL_TIME){
        ray = Ray(rayOrigin, rayDirection,ray.time + 1);
        color += rayColor(ray, vec3(x, y, ny), 0);
      }
      color = color / (FINAL_TIME + 1);
      ray.isMove = false;
      finalColor += color;
    }
    finalColor = finalColor / loop;
   }
   return finalColor;
  };

  atomic<int> progress(0);
  auto castRays = [renderPixel, &image, &progress] (uint y0, uint y1) mutable {
    for (uint y = y0; y < y1; y++) {
      for (uint x = 0; x < nx; x++) {
        vec3 color;
        // if (x != 260 || y != 320) {
        //   color = vec3(1.0, 1.0, 1.0);
        // }
        // else {
        //   color = renderPixel(x, y);
        // }
        color = renderPixel(x, y);
        image(x, y, 0) = color.x;
        image(x, y, 1) = color.y;
        image(x, y, 2) = color.z;
        progress += 1;
        if (progress % ((nx * ny)/10) == 0 ) {
          cout << "rendering " << progress / ((nx * ny)/10) << "0%" << endl;
        }
      }
    }
  };

// create tasks
  thread t[THREAD_NUM];
  int step = ny / THREAD_NUM;
  int overhead = ny % THREAD_NUM;
  int y_0 = 0;
  int y_1 = 0;
  for (int i = 0; i < THREAD_NUM; i++){
    int o = overhead > 0 ? 1 : 0;
    y_0 = y_1;
    y_1 = y_0 + step + o;
    t[i] = thread( castRays, y_0, y_1 );
    overhead--;
  }

  for (int i = 0; i < THREAD_NUM; i++){
    t[i].join();
  }

}

glm::dmat4 T1(double nx, double ny, double d) {
  dmat4 t1 = glm::translate(glm::dmat4(), glm::dvec3(-nx / 2, -ny / 2, d));
  return t1;
}


glm::dmat4 S2(double w, double h, double nx, double ny) {
  dmat4 s2 = glm::scale(glm::dmat4(), glm::dvec3(-w / nx, h / ny, 1));
  return s2;
}

glm::dmat4 R3(glm::dvec3 up, glm::dvec3 view) {
  dvec3 w = normalize(view);
  up = normalize(up);
  dvec3 u = normalize(cross(up, w));
  dvec3 v = cross(u, w);
  dmat4 r3({
    {u.x, u.y, u.z, 0},
    {v.x, v.y, v.z, 0},
    {w.x, w.y, w.z, 0},
    {0, 0, 0, 1}
  });
  return r3;
}

glm::dmat4 T4(glm::dvec3 eye){
  dmat4 t4({
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {eye.x, eye.y, eye.z, 1}
  });
  return t4;
}

glm::vec3 regularSampling(int x, int y, int size, double part, float * limit) {
  vec3 color = vec3();
  for( int i = 0; i < size; i++){
    for (int j = 0; j < size; j++){
      double x_1 = x + i / part;
      double y_1 = y + j / part;
      dvec4 world_point = scene_to_world_trans * dvec4(x_1, y_1, 0, 1);
      Ray new_ray = Ray(dvec4(eye_vec, 1), world_point - dvec4(eye_vec, 1), 0);
      vec3 new_color = rayColor(new_ray, vec3(x_1, y_1, ny), 0);
      limit[0] = std::min(new_color.x, limit[0]);
      limit[1] = std::min(new_color.y, limit[1]);
      limit[2] = std::min(new_color.z, limit[2]);
      limit[3] = std::max(new_color.x, limit[3]);
      limit[4] = std::max(new_color.y, limit[4]);
      limit[5] = std::max(new_color.z, limit[5]);
      color += new_color;
    }
  }
  vec3 result = color / (size * size);
  return result;
}

glm::vec3 adativeRegularSampling(int x, int y) {
  float * limit = new float[6];
  double part = 2.0;
  vec3 color = vec3();
  for (int i = 2; i < REGULAR_SAMPLING_MAX; ++i) {
    // reset limit value
    for (int j = 0; j < 3; ++j) { // reset min
      limit[j] = 1;
    }
    for (int j = 3; j < 6; ++j) { // reset max
      limit[j] = 0;
    }
    color = regularSampling(x, y, i, part, limit);
    float red_limit = limit[3] - limit[0];
    float green_limit = limit[4] - limit[1];
    float blue_limit = limit[5] - limit[2];
    if (red_limit < RED_THRESHOLDS &&
        blue_limit < BLUE_THRESHOLDS &&
        green_limit < GREEN_THRESHOLDS) {
      delete[] limit;
      return color;
    }
    part += 1.0;
  }
  delete[] limit;
  return color;
}


glm::vec3 rayColor(Ray ray, glm::vec3 pixel, int hit){
  vec3 color = vec3();
  vec3 point = vec3();
  Intersection hit_point = root_node->hit(ray);
  if (hit_point.is_intersect()) {
    color = color + hit_point.m_kd * ambient_light;
    vec4 point = ray.origin + hit_point.t * ray.direction;
    // regular color
    color += hit_point.m_coefficient.x * directLight(point, hit_point, ray.direction, ray.time);
    if (hit < MAX_HIT) {
      hit++;
      // reflection color
      if (REFLECTION) {
        Ray reflectionRay = ggReflection(ray, hit_point);
        vec3 reflectionColor = rayColor(reflectionRay, pixel, hit);
        color += hit_point.m_coefficient.y * reflectionColor;
      }
      // refraction color
      if (REFRACTION) {
        bool valid = true;
        Ray refractionRay = ggRefraction(ray, hit_point, valid);
        vec3 refractionColor;
        if(valid) {
          refractionColor = rayColor(refractionRay, pixel, hit);
        }
        color += hit_point.m_coefficient.z * refractionColor;
      }
    }
  } else {
    // background color
    color = vec3(0.988, 0.933, 0.921) - (pixel.y / pixel.z) * vec3(0.124, 0.306, 0.4745);
  }
  color = vec3(glm::min(color.x, 1.0f), glm::min(color.y, 1.0f), glm::min(color.z, 1.0f));
  return color;
}

glm::vec3 directLight(glm::dvec4 point, Intersection hit_point, glm::dvec4 direction, double time) {
  vec3 color;
  vec3 part_color;
  vec3 perturb;
  for (Light * light : light_beam) {
    int loop = AREA_LIGHT ? AREA_LIGHT_LOOP : 1;
    for (int i = 0; i < loop; ++i) {
      // soft shadow
      perturb.x = (rand() % 100 * 0.01) * LIGHT_WIDTH;
      perturb.z = (rand() % 100 * 0.01) * LIGHT_HEIGHT;
      dvec4 light_position = dvec4(light->position, 1);
      light_position = AREA_LIGHT ? light_position + dvec4(perturb, 1) : light_position;

      dvec4 light_direction = normalize(light_position - point);
      // calculate shadow ray
      Ray shadow_ray = Ray(point + EPSILON * light_direction, light_direction, time);
      double light_distance = length(light_position - shadow_ray.origin);

      Intersection shadow_hit_point = root_node->hit(shadow_ray);
      if (shadow_hit_point.is_intersect() && shadow_hit_point.t < light_distance) continue;

      vec3 diffuse = hit_point.m_kd * std::max(dot(light_direction, hit_point.N), 0.0) * light->colour;
      dvec4 reflect_ray = 2 * dot(light_direction, hit_point.N) * hit_point.N - light_direction;
      dvec4 point_to_viewer = -normalize(direction);
      vec3 specular = hit_point.m_ks * pow(std::max(dot(reflect_ray, point_to_viewer), 0.0), hit_point.m_shininess) * light->colour;
      part_color += diffuse + specular;
    }
    part_color = AREA_LIGHT ? part_color / AREA_LIGHT_LOOP : part_color;
    color += part_color;
  }
  return color;
}

Ray ggReflection(Ray & ray, Intersection & hitInfo) {
  dvec4 reflectionOrigin = ray.origin + hitInfo.t * ray.direction;
  dvec4 reflectionDirection = ray.direction - dvec4(2 * hitInfo.N * (dot(ray.direction, hitInfo.N)));
  reflectionOrigin = reflectionOrigin + EPSILON * reflectionDirection;
  Ray reflectionRay = Ray(reflectionOrigin, reflectionDirection, ray.time);
  return reflectionRay;
}

Ray ggRefraction(Ray & ray, Intersection & hitInfo, bool & valid) {
  vec4 refractionOrigin = ray.origin + hitInfo.t * ray.direction;
  dvec4 newN = hitInfo.N;
  double test = 1 - pow(hitInfo.ni / hitInfo.nt, 2) * (1 - pow(dot(ray.direction, newN), 2));
  if(test < 0) {
    valid = false;
  }
  vec4 refractionDirection = (-hitInfo.ni / hitInfo.nt * dot(ray.direction, newN) - abs(sqrt(1 - pow(hitInfo.ni / hitInfo.nt, 2) * (1 - pow(dot(ray.direction, hitInfo.N), 2))))) * hitInfo.N + hitInfo.ni / hitInfo.nt * ray.direction;
  refractionDirection = normalize(refractionDirection);
  refractionOrigin = refractionOrigin + EPSILON * refractionDirection;
  Ray refractionRay = Ray(refractionOrigin, refractionDirection, ray.time);
  return refractionRay;
}