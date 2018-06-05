#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <bitset>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();

	reset();
	initViewPort();
	initFrame();
	initCube();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//----------------------------------------------------------------------------------------
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

void A2::initFrame() {
	// init world frame
	world_frame[0][0] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	world_frame[0][1] = vec4(1.0f, 0.0f, 0.0f, 1.0f);

	world_frame[1][0] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	world_frame[1][1] = vec4(0.0f, 1.0f, 0.0f, 1.0f);


	world_frame[2][0] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	world_frame[2][1] = vec4(0.0f, 0.0f, 1.0f, 1.0f);

	// init model frame
	model_frame[0][0] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	model_frame[0][1] = vec4(1.0f, 0.0f, 0.0f, 1.0f);

	model_frame[1][0] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	model_frame[1][1] = vec4(0.0f, 1.0f, 0.0f, 1.0f);


	model_frame[2][0] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	model_frame[2][1] = vec4(0.0f, 0.0f, 1.0f, 1.0f);

	// init color
	world_color[0] = vec3(1.0f, 0.0f, 0.0f);
	world_color[1] = vec3(0.0f, 1.0f, 0.0f);
	world_color[2] = vec3(0.0f, 0.0f, 1.0f);

	model_color[0] = vec3(1.0f, 1.0f, 0.0f);
	model_color[1] = vec3(0.0f, 1.0f, 1.0f);
	model_color[2] = vec3(1.0f, 0.0f, 1.0f);
}

void A2::initViewPort() {
	viewport_point[0] = -1 + 2 * (0.05 * m_windowHeight) / m_windowHeight;
	viewport_point[1] = -1 + 2 * (0.05 * m_windowWidth) / m_windowWidth;
	viewport_point[2] = 1 - 2 * (0.05 * m_windowHeight) / m_windowHeight;
	viewport_point[3] = 1 - 2 * (0.05 * m_windowWidth) / m_windowWidth;
}

void A2::initCube() {
	cube[0][0]=vec4(1.0f, 1.0f, 1.0f, 1.0f);
	cube[0][1]=vec4(1.0f, 1.0f, -1.0f, 1.0f);

	cube[1][0]=vec4(1.0f, 1.0f, 1.0f, 1.0f);
	cube[1][1]=vec4(1.0f, -1.0f, 1.0f, 1.0f);

	cube[2][0]=vec4(1.0f, 1.0f, 1.0f, 1.0f);
	cube[2][1]=vec4(-1.0f, 1.0f, 1.0f, 1.0f);

	cube[3][0]=vec4(1.0f, -1.0f, -1.0f, 1.0f);
	cube[3][1]=vec4(1.0f, 1.0f, -1.0f, 1.0f);

	cube[4][0]=vec4(1.0f, -1.0f, -1.0f, 1.0f);
	cube[4][1]=vec4(1.0f, -1.0f, 1.0f, 1.0f);

	cube[5][0]=vec4(1.0f, -1.0f, -1.0f, 1.0f);
	cube[5][1]=vec4(-1.0f, -1.0f, -1.0f, 1.0f);

	cube[6][0]=vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	cube[6][1]=vec4( 1.0f, 1.0f, -1.0f, 1.0f);

	cube[7][0]=vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	cube[7][1]=vec4(-1.0f, 1.0f, 1.0f, 1.0f);

	cube[8][0]=vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	cube[8][1]=vec4(-1.0f, -1.0f, -1.0f, 1.0f);

	cube[9][0]=vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	cube[9][1]=vec4(1.0f, -1.0f, 1.0f, 1.0f);

	cube[10][0]=vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	cube[10][1]=vec4(-1.0f, 1.0f, 1.0f, 1.0f);

	cube[11][0]=vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	cube[11][1]=vec4(-1.0f, -1.0f, -1.0f, 1.0f);
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

void A2::setPerspectiveView(float curFieldOfView, float curNear, float curFar) {
	curFieldOfView = curFieldOfView < FIELD_OF_VIEW_MIN ? FIELD_OF_VIEW_MIN : curFieldOfView;
	curFieldOfView = curFieldOfView > FIELD_OF_VIEW_MAX ? FIELD_OF_VIEW_MAX : curFieldOfView;
	field_of_view = curFieldOfView;
	near = curNear;
	far = curFar;
	P = mat4(vec4(1/(tan(radians(field_of_view/2))), 0.0f, 0.0f, 0.0f),
					 vec4(0.0f, 1/tan(radians(field_of_view/2)), 0.0f, 0.0f),
					 vec4(0.0f, 0.0f, (far+near)/(far-near), 1.0f),
		       vec4(0.0f, 0.0f, -2*far*near/(far-near), 0.0f));
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & v0,   // Line Start (NDC coordinate)
		const glm::vec2 & v1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = v0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = v1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

void A2::transferDrawLine(glm::vec4 v1, glm::vec4 v2) {
	if (isClipTwoFace(v1, v2)) {
		v1 = P * v1;
		v2 = P * v2;
		if (isClipFourFace(v1, v2)) {
			fitViewPort(v1, v2);
			drawLine(vec2(v1.x/v1.w, v1.y/v1.w), vec2(v2.x/v2.w, v2.y/v2.w));
		}
	}
}

void A2::drawFrame() {
	for (int i = 0; i < FRAME_AXIS; ++i) {
		// draw world frame
		setLineColour(world_color[i]);
		vec4 v1 = world_frame[i][0];
		vec4 v2 = world_frame[i][1];
		v1 = V * v1;
		v2 = V * v2;
		transferDrawLine(v1, v2);

		// draw model frame
		setLineColour(model_color[i]);
		v1 = model_frame[i][0];
		v2 = model_frame[i][1];
		v1 = V * M * v1;
		v2 = V * M * v2;
		transferDrawLine(v1, v2);
	}
}


void A2::drawViewPort(){
	setLineColour(vec3(0.0));
	drawLine(vec2(viewport_point[0], viewport_point[1]), vec2(viewport_point[0], viewport_point[3]));
	drawLine(vec2(viewport_point[0], viewport_point[1]), vec2(viewport_point[2], viewport_point[1]));
	drawLine(vec2(viewport_point[0], viewport_point[3]), vec2(viewport_point[2], viewport_point[3]));
	drawLine(vec2(viewport_point[2], viewport_point[1]), vec2(viewport_point[2], viewport_point[3]));
}

void A2::drawCube(){
	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	for (int i = 0; i < 12; ++i) {
		vec4 v1 = cube[i][0];
		vec4 v2 = cube[i][1];
		v1 = V * M * scale(model_scale) * v1;
		v2 = V * M * scale(model_scale) * v2;
		transferDrawLine(v1, v2);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();
	drawViewPort();
	drawFrame();
	drawCube();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if( ImGui::Button( "Reset Application" ) ) {
			reset();
		}

		for (int i = 0; i < 7; ++i) {
			ImGui::PushID( i );
			ImGui::Text("%s", OPTION[i]);
			ImGui::SameLine();
			if( ImGui::RadioButton( "##Option", &current_mode, i ) ) {
			}
			ImGui::PopID();
		}
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

void A2::reset() {
	current_mode = 0;
	near = NEAR;
	far = FAR;
	field_of_view = FIELD_OF_VIEW;

	model_scale = vec3(1, 1, 1);
	model_rotate = vec3(0, 0, 0);
	model_translate = vec3(0, 0, 0);

	view_rotate = vec3(0, 0, 0);
	view_translate = vec3(0, 0, 0);

	M = mat4();

	V = inverse(mat4(
		vec4(1, 0, 0, 0), 
		vec4(0, 1, 0, 0), 
		vec4(0, 0, -1, 0), 
		vec4(0, 0, 10, 1)));

	setPerspectiveView(field_of_view, near, far);

}

bool A2::isClipTwoFace(vec4 &v1, vec4 &v2) {
	float b[2][2];
	bitset<2> c1;
	bitset<2> c2;

	// bn
	b[0][0] = -near + v1.z; 
	b[0][1] = -near + v2.z;

	// bf
	b[1][0] = far - v1.z; 
	b[1][1] = far - v2.z;

	for (int i = 0; i < 2; ++i) {
		c1[i] = (b[i][0] < 0);
		c2[i] = (b[i][1] < 0);
	}	// for

	// trivsal accept
	if((c1|c2) == 0) return true;
	// trivsal reject
	if((c1&c2) != 0) return false;

	// other case
	for (int i = 0; i < 2; i++){
		if(c1[i]|c2[i]){
			float t = b[i][0] / (b[i][0] - b[i][1]);
			vec4 newV = (1 - t) * v1 + t * v2;
			c1[i] ? v1 = newV : v2 = newV;
		}
	}
	return true;
}

bool A2::isClipFourFace(vec4 &v1, vec4 &v2) {
	float b[4][2];
	bitset<4> c1;
	bitset<4> c2;
	for (int i = 0; i < 4; ++i) {
		// bl
		b[0][0] =	v1.w + v1.x;
		b[0][1] =	v2.w + v2.x;
		// br
		b[1][0] =	v1.w - v1.x;
		b[1][1] = v2.w - v2.x;
		// bb
		b[2][0] =	v1.w + v1.y;
		b[2][1] =	v2.w + v2.y;
		// bt
		b[3][0] =	v1.w - v1.y;
		b[3][1] =	v2.w - v2.y;
		
		for (int i = 0; i < 4; ++i) {
			c1[i] = (b[i][0] < 0);
			c2[i] = (b[i][1] < 0);
		}	// for

		// trivsal accept
		if ((c1|c2) == 0) return true;
		// trivsal reject
		if ((c1&c2) != 0) return false;

		// other case
		if (c1[i]|c2[i]){					// if one of each is out of bound
			float t = b[i][0] / (b[i][0] - b[i][1]);
			vec4 newV = (1 - t) * v1 + t * v2;
			c1[i] ? v1 = newV : v2 = newV;
		}
	}
	return true;
}

void A2::fitViewPort(glm::vec4 &v1, glm::vec4 &v2) {
	v1.x = v1.x/v1.w; 
	v1.y = v1.y/v1.w; 
	v1.w = 1;

	v2.x = v2.x/v2.w; 
	v2.y = v2.y/v2.w; 
	v2.w = 1;

	float width = abs(viewport_point[0] - viewport_point[2]);
	float height = abs(viewport_point[1] - viewport_point[3]);

	float left = std::min(viewport_point[0], viewport_point[2]);
	float right = std::max(viewport_point[0], viewport_point[2]);
	float top = std::max(viewport_point[1], viewport_point[3]);
	float bottom = std::min(viewport_point[1], viewport_point[3]);


	v1.x = left + (((v1.x + 1)/2) * width);
	v1.y = bottom + (((v1.y + 1)/2) * height);
	v2.x = left + (((v2.x + 1)/2) * width);
	v2.y = bottom + (((v2.y + 1)/2) * height);
}

mat4 A2::scale(vec3 v) {
	mat4 result = mat4(
		vec4(v[0], 0.0f, 0.0f, 0.0f),
		vec4(0.0f, v[1], 0.0f, 0.0f),
		vec4(0.0f, 0.0f, v[2], 0.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f));

	return result;
}

mat4 A2::translate(vec3 theta) {
	mat4 result = mat4(
		vec4(1.0f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 1.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 1.0f, 0.0f),
		vec4(theta[0], theta[1], theta[2], 1.0f));

	return result;
}

mat4 A2::rotate(vec3 v){
	mat4 rotateX = mat4(
		vec4(1, 0, 0, 0),
		vec4(0, cos(v.x), sin(v.x), 0),
		vec4(0, -sin(v.x), cos(v.x), 0),
		vec4(0, 0, 0, 1));

	mat4 rotateY = mat4(
		vec4(cos(v.y), 0, -sin(v.y), 0),
		vec4(0, 1, 0, 0),
		vec4(sin(v.y), 0, cos(v.y), 0),
		vec4(0, 0, 0, 1));

	mat4 rotateZ= mat4(
		vec4(cos(v.z), sin(v.z), 0, 0),
		vec4(-sin(v.z), cos(v.z), 0, 0),
		vec4(0, 0, 1, 0),
		vec4(0, 0, 0, 1));

	return (rotateX)*(rotateY)*(rotateZ);
}
//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);
	if (!ImGui::IsMouseHoveringAnyWindow()) {
			double x_distance = xPos - mouse_x_current_pos;
			if (current_mode == 0) {	// view rotate
				mat4 rotateMat;
				if(ImGui::IsMouseDown(0))
					rotateMat = rotate(vec3(0.005 * x_distance, 0, 0));
				if(ImGui::IsMouseDown(2)) 
					rotateMat = rotate(vec3(0, 0.005* x_distance, 0));
				if(ImGui::IsMouseDown(1))
					rotateMat = rotate(vec3(0, 0, 0.005 * x_distance));
				V = inverse(rotateMat) * V;
			}
			if (current_mode == 1) {	// view translate
				mat4 translateMat;
				if(ImGui::IsMouseDown(0))
					translateMat = translate(vec3(0.02 * x_distance, 0, 0));
				if(ImGui::IsMouseDown(2)) 
					translateMat = translate(vec3(0, 0.02 * x_distance, 0));
				if(ImGui::IsMouseDown(1))
					translateMat = translate(vec3(0, 0, 0.02 * x_distance));
				V = inverse(translateMat) * V;
			}
			if (current_mode == 2) {	// prespective
				if(ImGui::IsMouseDown(0))
					setPerspectiveView(field_of_view + x_distance*0.02, near, far);
				if(ImGui::IsMouseDown(2)) 
					setPerspectiveView(field_of_view, near + x_distance*0.02, far);
				if(ImGui::IsMouseDown(1))
					setPerspectiveView(field_of_view, near, far + x_distance*0.02);
			}
			if (current_mode == 3) {	// model rotate
				mat4 rotateMat;
				if(ImGui::IsMouseDown(0))
					rotateMat = rotate(vec3(0.02 * x_distance, 0, 0));
				if(ImGui::IsMouseDown(2)) 
					rotateMat = rotate(vec3(0, 0.02 * x_distance, 0));
				if(ImGui::IsMouseDown(1))
					rotateMat = rotate(vec3(0, 0, 0.02 * x_distance));
				M = M * rotateMat;
			}
			if (current_mode == 4) {	// model translate
				mat4 translateMat;
				if(ImGui::IsMouseDown(0))
					translateMat = translate(vec3(0.02 * x_distance, 0, 0));
				if(ImGui::IsMouseDown(2)) 
					translateMat = translate(vec3(0, 0.02 * x_distance, 0));
				if(ImGui::IsMouseDown(1))
					translateMat = translate(vec3(0, 0, 0.02 * x_distance));
				M = M * translateMat;
			}
			if (current_mode == 5) {	// model scale
				if(ImGui::IsMouseDown(0))
					model_scale[0] = (1 + 0.02 * x_distance) * model_scale[0];
				if(ImGui::IsMouseDown(2)) 
					model_scale[1] = (1 + 0.02 * x_distance) * model_scale[1];
				if(ImGui::IsMouseDown(1))
					model_scale[2] = (1 + 0.02 * x_distance) * model_scale[2];
			}
			if (current_mode == 6) {
				if(ImGui::IsMouseDown(0)) {
					viewport_point[2] = (-1 + (2 * xPos/m_windowWidth) > 1) ? 1 : (-1 + (2 * xPos/m_windowWidth));
					viewport_point[3] = (1 - (2 * yPos/m_windowHeight) > 1) ? 1 : (1 - (2 * yPos/m_windowHeight));

					viewport_point[2] = viewport_point[2] < -1 ? -1 : viewport_point[2];
					viewport_point[3] = viewport_point[3] < -1 ? -1 : viewport_point[3];

				}
			}
			eventHandled = true;
	}

	mouse_x_current_pos = xPos;
	mouse_y_current_pos = yPos;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);
	if(actions == GLFW_PRESS){
		if (current_mode == 6) {
			viewport_point[0] = -1 + (2 * mouse_x_current_pos / m_windowWidth);
			viewport_point[1] = 1 - (2 * mouse_y_current_pos / m_windowHeight);
			viewport_point[2] = -1 + (2 * mouse_x_current_pos / m_windowWidth);
			viewport_point[3] = 1 - (2 * mouse_y_current_pos / m_windowHeight);
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if ( action == GLFW_PRESS ) {
		if ( key == GLFW_KEY_Q ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if ( key == GLFW_KEY_A ) {
			reset();
		}
		if ( key == GLFW_KEY_O ) {	// rotate view
			current_mode = 0;
		}
		if ( key == GLFW_KEY_N ) {	// translate view
			current_mode = 1;
		}
		if ( key == GLFW_KEY_P ) {	// perspective
			current_mode = 2;
		}
		if ( key == GLFW_KEY_R ) {	// rotate model
			current_mode = 3;
		}
		if ( key == GLFW_KEY_T ) {	// translate model
			current_mode = 4;
		}
		if ( key == GLFW_KEY_S ) {	// scale model
			current_mode = 5;
		}
		if ( key == GLFW_KEY_V ) {	// viewport
			current_mode = 6;
		}
		eventHandled = true;
	}

	return eventHandled;
}
