#include "CSGNode.hpp"
#include "PhongMaterial.hpp"
#include <glm/gtx/io.hpp>

using namespace glm;
using namespace std;

//---------------------------------------------------------------------------------------

CSGNode::CSGNode(
  const std::string & name, GeometryNode *g1, GeometryNode *g2, int operation )
  : SceneNode( name )
  , g1( g1 )
  , g2( g2 )
  , operation(operation)
{
  m_nodeType = NodeType::CSGNode;
}

Intersection CSGNode::hit(Ray & ray)
{

  Ray ray1 = ray;
  Ray ray2 = ray;

  ray1.csgInfo = NULL;
  ray2.csgInfo = NULL;

  Intersection result_1 = g1->hit(ray1);
  Intersection result_2 = g2->hit(ray2);

  Intersection result = Intersection();

  if(!ray1.csgInfo && !ray2.csgInfo) return result; // both null
  double pt1_front_t = ray1.csgInfo ? ray1.csgInfo->front_t : DBL_MAX;
  double pt1_back_t = ray1.csgInfo ? ray1.csgInfo->back_t : DBL_MAX;
  dvec4 pt1_front_N = ray1.csgInfo ? ray1.csgInfo->front_N : dvec4();
  dvec4 pt1_back_N = ray1.csgInfo ? ray1.csgInfo->back_N : dvec4();



  double pt2_front_t = ray2.csgInfo ? ray2.csgInfo->front_t : DBL_MAX;
  double pt2_back_t = ray2.csgInfo ? ray2.csgInfo->back_t : DBL_MAX;
  dvec4 pt2_front_N = ray2.csgInfo ? ray2.csgInfo->front_N : dvec4();
  dvec4 pt2_back_N = ray2.csgInfo ? ray2.csgInfo->back_N : dvec4();


  if(operation == 0) { //union
    double neart = glm::min(pt1_front_t, pt2_front_t);
    dvec4 nearn = (neart == (pt1_front_t)) ? pt1_front_N : pt2_front_N;
    dvec3 nearkd = (neart == (pt1_front_t)) ? result_1.m_kd : result_2.m_kd;
    result.t = neart;
    result.N = nearn;
    result.m_kd = nearkd;
  } else if(operation == 1){ //intersect
    double farft = glm::max(pt1_front_t, pt2_front_t);
    double nearbt = glm::min(pt1_back_t, pt2_back_t);
    if (farft > nearbt) return result;
    dvec4 farfn = (farft == (ray1.csgInfo ? pt1_front_t : -DBL_MAX)) ? pt1_front_N : pt2_front_N;
    dvec3 farkd = (farft == (ray1.csgInfo ? pt1_front_t : -DBL_MAX)) ? result_1.m_kd : result_2.m_kd;
    result.t = farft;
    result.N = farfn;
    result.m_kd = farkd;
  } else { // difference
    if(!ray1.csgInfo) return result;
    if(!ray2.csgInfo || pt1_front_t < pt2_front_t){
      result.t = pt1_front_t;
      result.N = pt1_front_N;
      result.m_kd = result_1.m_kd;
    } else {
      if(pt1_back_t > pt2_back_t){
        result.t = pt2_back_t;
        result.N = -pt2_back_N;
        result.m_kd = result_2.m_kd;
      }
    }
  }
  return result;
}
