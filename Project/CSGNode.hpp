#pragma once

#include "GeometryNode.hpp"
#include "Primitive.hpp"
#include "Intersection.hpp"
#include "Material.hpp"
#include "Ray.hpp"

class CSGNode : public SceneNode {
public:

  CSGNode( const std::string & name, GeometryNode *g1, GeometryNode *g2, int operation);

  GeometryNode *g1;
  GeometryNode *g2;
  std::string name;
  int operation;

  Intersection hit(Ray & ray);
};
