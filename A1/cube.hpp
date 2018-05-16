#ifndef	__CUBE_HPP__
#define __CUBE_HPP__

#include <glm/glm.hpp>

#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

class Cube {
public:
	int colour = 0;

	Cube(glm::vec3 origin_position);

	void uploadVboData();
	
	void uploadVaoData(const ShaderProgram &shader);
	
	void uploadEboData();

	void uploadData(const ShaderProgram &shader);
};

#endif