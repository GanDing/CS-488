#include "cube.hpp"

using namespace std;

Cube::Cube(glm::vec3 origin_position){

	// define the vertice for the cube
	vertices = {
		origin_position,																	// vertex #0
		origin_position + glm::vec3( 1.0f, 0.0f, 0.0f ),	// vertex #1
		origin_position + glm::vec3( 0.0f, -1.0f, 0.0f ),	// vertex #2
		origin_position + glm::vec3( 1.0f, -1.0f, 0.0f ),	// vertex #3
		origin_position + glm::vec3( 0.0f, 0.0f, -1.0f ),	// vertex #4
		origin_position + glm::vec3( 1.0f, 0.0f, -1.0f ),	// vertex #5
		origin_position + glm::vec3( 0.0f, -1.0f, -1.0f ),// vertex #6
		origin_position + glm::vec3( 1.0f, -1.0f, -1.0f )	// vertex #7
	};

	trangle_position = {
		// front face 
		0, 2, 3,	// bottom left 
		3, 1, 0,	// upper right

		// left face
		4, 6, 2, 	// bottom left
		2, 0, 4, 	// upper right

		// right face
		1, 4, 7, 	// bottom left
		7, 5, 1,	// upper right

		// upper face
		4, 0, 1, 	// bottom left
		1, 5, 4,	// upper right

		// bottom face
		6, 3, 4, 	// bottom left
		4, 7, 6,	// upper right

		// back face
		4, 6, 7, 	// bottom left
		7, 5, 4,	// upper right
	};
}


void Cube::uploadVboData() {

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size() , vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CHECK_GL_ERRORS;

}
	
void Cube::uploadVaoData(const ShaderProgram &shader) {

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	GLint positionAttribLocation = shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	
	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	CHECK_GL_ERRORS;
	
}
	
void Cube::uploadEboData() {

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * layout.size(), layout.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CHECK_GL_ERRORS;

}

void Cube::uploadData(const ShaderProgram &shader) {

	uploadVboData();
	uploadEboData();
	uploadVaoData(shader);
	
}