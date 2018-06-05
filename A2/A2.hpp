#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;
const int FRAME_AXIS = 3;
const int FAR = 20;
const int NEAR = 5;
const int FIELD_OF_VIEW = 30;
const int FIELD_OF_VIEW_MIN = 5;
const int FIELD_OF_VIEW_MAX = 150;
// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();
	const char* OPTION [7] = {"Rotate View(O)    \t", "Translate View(N) \t", "Perspective(P)    \t", "Rotate Model(R)   \t", "Translate Model(T)\t", "Scale Model(S)    \t", "Viewport(V)       \t"};

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

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void setPerspectiveView(float curFieldOfView, float curNear, float curFar);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);

	void transferDrawLine(glm::vec4 v1, glm::vec4 v2);
	void reset();

	void initFrame();
	void initViewPort();
	void initCube();

	void drawFrame();
	void drawViewPort();
	void drawCube();

	bool isClipTwoFace(glm::vec4 &v1, glm::vec4 &v2);
	bool isClipFourFace(glm::vec4 &v1, glm::vec4 &v2);

	void fitViewPort(glm::vec4 &v1, glm::vec4 &v2);

	glm::mat4 scale(glm::vec3 v);
	glm::mat4 translate(glm::vec3 theta);
	glm::mat4 rotate(glm::vec3 v);

	glm::mat4 V;
	glm::mat4 M;
	glm::mat4 P;

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;
	int current_mode;
	glm::vec3 m_currentLineColour;

	float viewport_point[4];

	glm::vec4 cube[12][2];
	glm::vec4 world_frame[3][2];
	glm::vec4 model_frame[3][2];
	glm::vec3 world_color[3];
	glm::vec3 model_color[3];

	glm::vec3 model_scale;
	glm::vec3 model_rotate;
	glm::vec3 model_translate;

	glm::vec3 view_rotate;
	glm::vec3 view_translate;

	float near;
	float far;
	float field_of_view;

	double mouse_x_current_pos;
	double mouse_y_current_pos;
};
