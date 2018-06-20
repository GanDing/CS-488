#include <glm/ext.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <atomic>

#include "A4.hpp"

#define DISTANCE 10.0
#define REGULAR_SAMPLING true
#define REGULAR_SAMPLING_SIZE 4
#define THREAD_NUM 4
#define MAX_HIT 1
#define EPSILON 0.0001

using namespace glm;
using namespace std;

SceneNode * root_node;
vec3 ambient_light;
dmat4 scene_to_world_trans;
list<Light *> light_beam;
vec3 eye_vec; 
size_t nx;
size_t ny;

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

  if (REGULAR_SAMPLING) {
    cout << "super sampling " << endl;
  }

	auto renderPixel = [] (int x, int y) -> vec3 {
   vec3 color;
   if (REGULAR_SAMPLING) {
     color = regularSampling(x, y);
   } else {
     dvec4 world_point = scene_to_world_trans * dvec4(x, y, 0, 1);
     Ray ray = Ray(dvec4(eye_vec, 1), world_point - dvec4(eye_vec, 1));
     color = rayColor(ray, light_beam, MAX_HIT, vec3(x, y, ny));
   }
   return color;
	};

  atomic<int> progress(0);
  auto castRays = [renderPixel, &image, &progress] (uint y0, uint y1) mutable {
    for (uint y = y0; y < y1; y++) {
      for (uint x = 0; x < nx; x++) {
        vec3 color;
        // if (x !=95 || y != 160) {
        //   color = vec3(1.0, 1.0, 1.0);
        // }
        // else {
        //   color = oneRay(x, y);  
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
  int y0 = 0;
  int y1 = 0;
  for (int i = 0; i < THREAD_NUM; i++){
    int o = overhead > 0 ? 1 : 0;
    y0 = y1;
    y1 = y0 + step + o;
    t[i] = thread( castRays, y0, y1 );
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

glm::vec3 regularSampling(int x, int y) {
  vec3 color = vec3();
  for( int i = 0; i < REGULAR_SAMPLING_SIZE; i++){
    for (int j = 0; j < REGULAR_SAMPLING_SIZE; j++){
      double x_1 = x + i / REGULAR_SAMPLING_SIZE;
      double y_1 = y + j / REGULAR_SAMPLING_SIZE;
      dvec4 world_point = scene_to_world_trans * dvec4(x_1, y_1, 0, 1);
      Ray new_ray = Ray(dvec4(eye_vec, 1), world_point - dvec4(eye_vec, 1));
      vec3 new_color = rayColor(new_ray, light_beam, MAX_HIT, vec3(x_1, y_1, ny));
      color += new_color;
    }
  }
  vec3 result = color / REGULAR_SAMPLING_SIZE / REGULAR_SAMPLING_SIZE;
  return result;
}


glm::vec3 rayColor(Ray ray, const std::list<Light *> & lights, int maxHits, glm::vec3 pixel){
	vec3 color = vec3();
	vec3 point = vec3();
	Intersection hit_point = root_node->hit(ray);
	if (hit_point.is_intersect()) {
		color = color + hit_point.phong_material.m_kd * ambient_light;
		vec4 point = ray.origin + hit_point.t * ray.direction;
    color += directLight(point, hit_point, ray.direction);
	} else {
    // background color
		color = vec3(0.988, 0.933, 0.921) - (pixel.y / pixel.z) * vec3(0.124, 0.306, 0.4745);
	}
	return color;
}

glm::vec3 directLight(glm::dvec4 point, Intersection hit_point, glm::dvec4 direction) {
  vec3 color;
  for (Light * light : light_beam) {
    dvec4 light_position = dvec4(light->position, 1);
    dvec4 light_direction = normalize(light_position - point);
    Ray shadow_ray = Ray(point + EPSILON * light_direction, light_direction);
    double light_distance = length(light_position - shadow_ray.origin);

    Intersection shadow_hit_point = root_node->hit(shadow_ray);
    if (shadow_hit_point.is_intersect() && shadow_hit_point.t < light_distance) continue;

    vec3 diffuse = hit_point.phong_material.m_kd * std::max(dot(light_direction, hit_point.N), 0.0) * light->colour;
    dvec4 reflect_ray = 2 * dot(light_direction, hit_point.N) * hit_point.N - light_direction;
    dvec4 point_to_viewer = -normalize(direction);
    vec3 specular = hit_point.phong_material.m_ks * pow(std::max(dot(reflect_ray, point_to_viewer), 0.0), hit_point.phong_material.m_shininess) * light->colour;
    color += diffuse + specular;
  }
  return color;
}











