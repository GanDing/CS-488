#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"

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
);

glm::dmat4 T1(double nx, double ny, double d);

glm::dmat4 S2(double w, double h, double nx, double ny);

glm::dmat4 R3(glm::dvec3 up, glm::dvec3 view);

glm::dmat4 T4(glm::dvec3 eye);

glm::vec3 rayColor(Ray ray, const std::list<Light *> & lights, int maxHits, glm::vec3 pixel);

glm::vec3 regularSampling(int x, int y);

glm::vec3 directLight(glm::dvec4 point, Intersection hit_point, glm::dvec4 direction);

glm::vec3 renderPixel(int x, int y);

void castRays(Image & image);
