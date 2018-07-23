#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

#include "Mesh.hpp"

using namespace glm;

#define BOUNDING_VOLUMN_TEST false

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );

  std::string file = "";
  if(ifs.fail()){
    file = "Assets/" + fname;
    ifs = std::ifstream(file.c_str());
  }
  if(ifs.fail()){
    std::cout << "fail: " << file << std::endl;
  }

	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
      if (!init) {
        max_size = vec3(vx, vy, vz);
        min_size = vec3(vx, vy, vz);
        init = true;
      } else {
        min_size.x = std::min(vx, min_size.x);
        min_size.y = std::min(vy, min_size.y);
        min_size.z = std::min(vz, min_size.z);

        max_size.x = std::max(vx - min_size.x, max_size.x);
        max_size.y = std::max(vy - min_size.y, max_size.y);
        max_size.z = std::max(vz - min_size.z, max_size.z);

      }
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}

  bounding_box = NonhierBox(min_size, max_size);
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*

  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

Intersection Mesh::hit(Ray ray){
  if (BOUNDING_VOLUMN_TEST) {
    Intersection hit_point = bounding_box.hit(ray);
    if (hit_point.is_intersect())
      return hit_point;
    else
      return Intersection();
  }
  double t = -1.0;
  double baryPosition = 0.0;
  Intersection result = Intersection();
  for (Triangle face : m_faces) {
    bool is_hit = ray_intersect_triangle(ray, m_vertices[face.v1], m_vertices[face.v2], m_vertices[face.v3], baryPosition);
    if (is_hit) {
      t = baryPosition;
      if (!result.is_intersect() || t < result.t) {
        result.t = t;
        vec3 normal = normalize(cross(
          m_vertices[face.v2] - m_vertices[face.v1],
          m_vertices[face.v3] - m_vertices[face.v1]));
        dvec4 N = dvec4(normal, 0);
        result.N = N;
      }
    }
  }
  if(!result.is_intersect()) {
    return Intersection();
  }
  return result;
}