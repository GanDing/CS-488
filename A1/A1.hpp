#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "grid.hpp"

const size_t DIM = 20;
const int CUBE_VERTEX = 8;
const int CUBE_TRIANGLES = 36;
const int CUBE_EDGES = 24;
const int INDICATOR_VERTEX = 6;
const int INDICATOR_TRIANGLES = 24;
const int INDICATOR_EDGES = 24;
const int MAX_CUBE = 10;

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

private:
	void moveActive(int x_pos, int z_pos);

	void initGrid();
	void initCube(int x_pos, int y_pos, int z_pos);
	void initIndicator();

	void drawCube(GLint x_pos, GLint z_pos);
	void drawIndicator();

	void reset();

	void generateCubeVertices(GLfloat x_pos, GLfloat y_pos, GLfloat z_pos, glm::vec3 cube_vertices[CUBE_VERTEX]);
	void generateCubeFaces(glm::vec3 cube_vertices[CUBE_VERTEX], glm::vec3 cube_triangles[CUBE_TRIANGLES]);
	void generateCubeEdges(glm::vec3 cube_vertices[CUBE_VERTEX], glm::vec3 cube_edges[CUBE_EDGES]);

	void generateIndicatorVertices(GLfloat x_pos, GLfloat y_pos, GLfloat z_pos, glm::vec3 indicator_vertices[INDICATOR_VERTEX]);
	void generateIndicatorFaces(glm::vec3 indicator_vertices[INDICATOR_VERTEX], glm::vec3 indicator_triangles[INDICATOR_TRIANGLES]);
	void generateIndicatorEdges (glm::vec3 indicator_vertices[INDICATOR_VERTEX], glm::vec3 indicator_edges[INDICATOR_EDGES]);

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Fields related to cube geometry.
	GLuint m_cube_vao[DIM][MAX_CUBE][DIM][2];
  GLuint m_cube_vbo[DIM][MAX_CUBE][DIM][2];

  // Fields related to indicator geometry.
	GLuint m_indicator_vao[2]; // Vertex Array Object
	GLuint m_indicator_vbo[2]; // Vertex Buffer Object

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	// Default colour set
	float * default_colour[8];

	// Press status flag
	bool shift_press;
	bool symmetric_press;
	bool border_press;

	int current_col;

	// Grid transformation variable
	float m_shape_size;
	float m_shape_rotation;
	float m_shape_rotation_x;

	double mouse_x_current_pos;

	// Grid infomation
	std::vector<std::vector<std::pair<GLint, GLint>>> cell_info;
	std::pair<GLint, GLint> active_cell;
};
