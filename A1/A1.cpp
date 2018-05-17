#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <algorithm>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 ), 
	m_shape_size ( 1.0f ),
	m_shape_rotation( 0.0f ),
	m_shape_rotation_x( 0.0f ),
	shift_press( false ),
	symmetric_press( false ),
	border_press( true )
{
	// init the default colours
	default_colour[0] = new float[3]{ 0.00f, 0.75f, 0.94f };
	default_colour[1] = new float[3]{ 0.85f, 0.45f, 0.34f };
	default_colour[2] = new float[3]{ 0.69f, 1.00f, 0.69f };
	default_colour[3] =	new float[3]{ 0.99f, 0.69f, 1.00f };
	default_colour[4] =	new float[3]{ 0.92f, 0.76f, 0.54f };
	default_colour[5] =	new float[3]{ 0.99f, 0.75f, 0.29f };
	default_colour[6] = new float[3]{ 0.00f, 0.00f, 0.00f };
	default_colour[7] =	new float[3]{ 1.00f, 1.00f, 1.00f };

	// init the active cell position
	active_cell.first = 0;
	active_cell.second = 0;

	// init the height for each cell
	for (int i = 0; i < DIM; ++i) {
		std::vector<std::pair<GLint, GLint>> new_cell_info_set;

		for (int j = 0; j < DIM; ++j) {
			std::pair<GLint, GLint> new_pair;
			new_pair.first = 0;
			new_pair.second = 0;
			new_cell_info_set.push_back(new_pair);
		}

		cell_info.push_back(new_cell_info_set);
	}
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
	for (int i = 0; i < 8; ++i) {
		delete[] default_colour[i];
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();

	for (int i = 0; i < DIM; ++i) {
		for (int j = 0; j < DIM; ++j) {
			for (int k = 0; k < MAX_CUBE; ++k) {
				initCube(i, k, j);
			}
		}
	}

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::initCube(int x_pos, int y_pos, int z_pos) 
{
	vec3 cube_vertices[CUBE_VERTEX];
	vec3 cube_triangles[CUBE_TRIANGLES];
	GLfloat cur_height = MAX_CUBE;
	vec3 cube_edges[CUBE_EDGES];
	generateCubeVertices(x_pos, y_pos, z_pos, cube_vertices);
	generateCubeFaces(cube_vertices, cube_triangles);
	generateCubeEdges(cube_vertices, cube_edges);

	//	init triangle
	glGenVertexArrays(1, &(m_cube_vao[x_pos][y_pos][z_pos][0]) ); 
	glBindVertexArray( m_cube_vao[x_pos][y_pos][z_pos][0] );

	glGenBuffers( 1, &(m_cube_vbo[x_pos][y_pos][z_pos][0]) );
	glBindBuffer( GL_ARRAY_BUFFER, m_cube_vbo[x_pos][y_pos][z_pos][0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cube_triangles),
		cube_triangles, GL_STATIC_DRAW );

	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );


	//	init edges
	glGenVertexArrays(1, &(m_cube_vao[x_pos][y_pos][z_pos][1]) ); 
	glBindVertexArray( m_cube_vao[x_pos][y_pos][z_pos][1] );

	glGenBuffers( 1, &(m_cube_vbo[x_pos][y_pos][z_pos][1]) );
	glBindBuffer( GL_ARRAY_BUFFER, m_cube_vbo[x_pos][y_pos][z_pos][1] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cube_edges),
		cube_edges, GL_STATIC_DRAW );

	posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void A1::initIndicator() {
	GLfloat x_pos = active_cell.first;
	GLfloat z_pos = active_cell.second;
	GLfloat y_pos = cell_info[x_pos][z_pos].first;
	vec3 indicator_vertices[INDICATOR_VERTEX];
	vec3 indicator_triangles[INDICATOR_TRIANGLES];
	vec3 indicator_edges[INDICATOR_EDGES];

	generateIndicatorVertices(x_pos, y_pos, z_pos, indicator_vertices);
	generateIndicatorFaces(indicator_vertices, indicator_triangles);
	generateIndicatorEdges(indicator_vertices, indicator_edges);
	// init triangles
	glGenVertexArrays(1, &(m_indicator_vao[0]) ); 
	glBindVertexArray( m_indicator_vao[0] );

	glGenBuffers( 1, &(m_indicator_vbo[0]) );
	glBindBuffer( GL_ARRAY_BUFFER, m_indicator_vao[0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(indicator_triangles),
		indicator_triangles, GL_STATIC_DRAW );
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// init lines
	glGenVertexArrays(1, &(m_indicator_vao[1]) ); 
	glBindVertexArray( m_indicator_vao[1] );

	glGenBuffers( 1, &(m_indicator_vbo[1]) );
	glBindBuffer( GL_ARRAY_BUFFER, m_indicator_vao[1] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(indicator_edges),
		indicator_edges, GL_STATIC_DRAW );
	posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}


//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		if( ImGui::Button( "Reset Application" ) ) {
			reset();
		}

		for (int i = 0; i < 8; ++i) {
			ImGui::PushID( i );
			ImGui::ColorEdit3( "##Colour", default_colour[i] );
			ImGui::SameLine();
			if( ImGui::RadioButton( "##Col", &current_col, i ) ) {
				cell_info[active_cell.first][active_cell.second].second = i;
			}
			ImGui::PopID();
		}

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::scale( W, vec3( m_shape_size, m_shape_size, m_shape_size ) );
	W = glm::rotate( W, m_shape_rotation, vec3(0, 1, 0) );
	W = glm::rotate( W, m_shape_rotation_x, vec3(1, 0, 0) );
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		for (int i = 0; i < DIM; ++i) {
			for (int j = 0; j < DIM; ++j) {
				drawCube(i, j);
			}
		}

		drawIndicator();

	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );
}


void A1::drawCube(GLint x_pos, GLint z_pos) 
{
	int height = cell_info[x_pos][z_pos].first;
	int colour = cell_info[x_pos][z_pos].second;
	for (int i = 0; i < height; ++i){
		glBindVertexArray( m_cube_vao[x_pos][i][z_pos][0] );
		glUniform3f( col_uni, default_colour[colour][0], default_colour[colour][1], default_colour[colour][2] );
		glDrawArrays( GL_TRIANGLES , 0, CUBE_TRIANGLES);
		if (border_press) {
			glBindVertexArray( m_cube_vao[x_pos][i][z_pos][1] );
			glUniform3f( col_uni, 0, 0, 0 );
			glDrawArrays( GL_LINES , 0, CUBE_EDGES);
		}
	}
	
}

void A1::drawIndicator() {
	initIndicator();

	glDisable( GL_DEPTH_TEST );
	mat4 W;

	W = glm::scale( W, vec3( m_shape_size, m_shape_size, m_shape_size ) );
	W = glm::rotate( W, m_shape_rotation, vec3(0, 1, 0) );
	W = glm::rotate( W, m_shape_rotation_x, vec3(1, 0, 0) );
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	glBindVertexArray( m_indicator_vao[0] );
	glUniform3f( col_uni, 0.90f, 0.93f, 0.96f );
	glDrawArrays( GL_TRIANGLES , 0, INDICATOR_TRIANGLES);	

	glBindVertexArray( m_indicator_vao[1] );
	glUniform3f( col_uni, 0, 0, 0 );
	glDrawArrays( GL_LINES , 0, INDICATOR_EDGES);	

	glEnable( GL_DEPTH_TEST );
}

void A1::generateCubeVertices(GLfloat x_pos, GLfloat y_pos, GLfloat z_pos, vec3 cube_vertices[CUBE_VERTEX]) {
	vec3 vertex_0 = vec3( x_pos, 				y_pos, 				 z_pos );
	vec3 vertex_1 = vec3( 1.0f + x_pos, y_pos, 				 z_pos );
	vec3 vertex_2 = vec3( x_pos, 				1.0f + y_pos , z_pos );
	vec3 vertex_3 = vec3( 1.0f + x_pos, 1.0f + y_pos,  z_pos );
	vec3 vertex_4 = vec3( x_pos, 				y_pos, 				 1.0f + z_pos );
	vec3 vertex_5 = vec3( 1.0f + x_pos, y_pos, 				 1.0f + z_pos );
	vec3 vertex_6 = vec3( x_pos, 				1.0f + y_pos,	 1.0f + z_pos );
	vec3 vertex_7 = vec3( 1.0f + x_pos, 1.0f + y_pos,	 1.0f + z_pos );

	cube_vertices[0] = vertex_0; 
	cube_vertices[1] = vertex_1;
	cube_vertices[2] = vertex_2; 
	cube_vertices[3] = vertex_3;
	cube_vertices[4] = vertex_4; 
	cube_vertices[5] = vertex_5;
	cube_vertices[6] = vertex_6;
	cube_vertices[7] = vertex_7;
}

void A1::generateCubeFaces(vec3 cube_vertices[CUBE_VERTEX], vec3 cube_triangles[CUBE_TRIANGLES]) {
	// front face
	cube_triangles[0] = cube_vertices[0]; cube_triangles[1] = cube_vertices[1]; cube_triangles[2] = cube_vertices[3];
	cube_triangles[3] = cube_vertices[0]; cube_triangles[4] = cube_vertices[2]; cube_triangles[5] = cube_vertices[3];
	// back face
	cube_triangles[6] = cube_vertices[4]; cube_triangles[7] = cube_vertices[5]; cube_triangles[8] = cube_vertices[7];
	cube_triangles[9] = cube_vertices[4]; cube_triangles[10] = cube_vertices[6]; cube_triangles[11] = cube_vertices[7];
	// left face
	cube_triangles[12] = cube_vertices[0]; cube_triangles[13] = cube_vertices[2]; cube_triangles[14] = cube_vertices[6];
	cube_triangles[15] = cube_vertices[0]; cube_triangles[16] = cube_vertices[4]; cube_triangles[17] = cube_vertices[6];
	// right face
	cube_triangles[18] = cube_vertices[1]; cube_triangles[19] = cube_vertices[3]; cube_triangles[20] = cube_vertices[7]; 
	cube_triangles[21] = cube_vertices[1]; cube_triangles[22] = cube_vertices[5]; cube_triangles[23] = cube_vertices[7];
	// top face
	cube_triangles[24] = cube_vertices[0]; cube_triangles[25] = cube_vertices[1]; cube_triangles[26] = cube_vertices[5];
	cube_triangles[27] = cube_vertices[0]; cube_triangles[28] = cube_vertices[4]; cube_triangles[29] = cube_vertices[5];
	// bottom face
	cube_triangles[30] = cube_vertices[2]; cube_triangles[31] = cube_vertices[3]; cube_triangles[32] = cube_vertices[7];
	cube_triangles[33] = cube_vertices[2]; cube_triangles[34] = cube_vertices[6]; cube_triangles[35] = cube_vertices[7];
}


void A1::generateCubeEdges(glm::vec3 cube_vertices[CUBE_VERTEX], glm::vec3 cube_edges[CUBE_EDGES]){

	// upper square
	cube_edges[0] = cube_vertices[2]; 
	cube_edges[1] = cube_vertices[3]; 
	cube_edges[2] = cube_vertices[3];
	cube_edges[3] = cube_vertices[7]; 
	cube_edges[4] = cube_vertices[7]; 
	cube_edges[5] = cube_vertices[6];
	cube_edges[6] = cube_vertices[6]; 
	cube_edges[7] = cube_vertices[2]; 
	// middle column
	cube_edges[8] = cube_vertices[6];
	cube_edges[9] = cube_vertices[4];
	cube_edges[10] = cube_vertices[7]; 
	cube_edges[11] = cube_vertices[5];
	cube_edges[12] = cube_vertices[3]; 
	cube_edges[13] = cube_vertices[1]; 
	cube_edges[14] = cube_vertices[2];
	cube_edges[15] = cube_vertices[0]; 
	// bottom square
	cube_edges[16] = cube_vertices[4]; 
	cube_edges[17] = cube_vertices[5];
	cube_edges[18] = cube_vertices[5];
	cube_edges[19] = cube_vertices[1]; 
	cube_edges[20] = cube_vertices[1]; 
	cube_edges[21] = cube_vertices[0]; 
	cube_edges[22] = cube_vertices[0]; 
	cube_edges[23] = cube_vertices[4];
}

void A1::generateIndicatorVertices(GLfloat x_pos, GLfloat y_pos, GLfloat z_pos, vec3 indicator_vertices[INDICATOR_VERTEX]) {
	vec3 vertex_0 = vec3( x_pos, 				1.0f + y_pos,  z_pos );
	vec3 vertex_1 = vec3( 1.0f + x_pos, 1.0f + y_pos,  z_pos );
	vec3 vertex_2 = vec3( x_pos, 				1.0f + y_pos,  1.0f + z_pos );
	vec3 vertex_3 = vec3( 1.0f + x_pos,	1.0f + y_pos,  1.0f + z_pos );
	vec3 vertex_4 = vec3( 0.5f + x_pos, 0.25f + y_pos, 0.5f + z_pos );
	vec3 vertex_5 = vec3( 0.5f + x_pos, 1.75f + y_pos, 0.5f + z_pos );

	indicator_vertices[0] = vertex_0; 
	indicator_vertices[1] = vertex_1;
	indicator_vertices[2] = vertex_2; 
	indicator_vertices[3] = vertex_3;
	indicator_vertices[4] = vertex_4; 
	indicator_vertices[5] = vertex_5;
}

void A1::generateIndicatorFaces(vec3 indicator_vertices[INDICATOR_VERTEX], vec3 indicator_triangles[INDICATOR_TRIANGLES]) {
	// upper front
	indicator_triangles[0] = indicator_vertices[5]; indicator_triangles[1] = indicator_vertices[2]; indicator_triangles[2] = indicator_vertices[3];
	// bottom front
	indicator_triangles[3] = indicator_vertices[2]; indicator_triangles[4] = indicator_vertices[4]; indicator_triangles[5] = indicator_vertices[3];
	// upper left
	indicator_triangles[6] = indicator_vertices[5]; indicator_triangles[7] = indicator_vertices[0]; indicator_triangles[8] = indicator_vertices[2];
	// bottom left
	indicator_triangles[9] = indicator_vertices[0]; indicator_triangles[10] = indicator_vertices[4]; indicator_triangles[11] = indicator_vertices[2];
	// upper right
	indicator_triangles[12] = indicator_vertices[5]; indicator_triangles[13] = indicator_vertices[3]; indicator_triangles[14] = indicator_vertices[1];
	// bottom right
	indicator_triangles[15] = indicator_vertices[3]; indicator_triangles[16] = indicator_vertices[4]; indicator_triangles[17] = indicator_vertices[1];
	// upper back 
	indicator_triangles[18] = indicator_vertices[1]; indicator_triangles[19] = indicator_vertices[0]; indicator_triangles[20] = indicator_vertices[5]; 
	// bottom back
	indicator_triangles[21] = indicator_vertices[0]; indicator_triangles[22] = indicator_vertices[1]; indicator_triangles[23] = indicator_vertices[4];
}


void A1::generateIndicatorEdges(vec3 indicator_vertices[INDICATOR_VERTEX], vec3 indicator_edges[INDICATOR_EDGES]) {
	// upper four
	indicator_edges[0] = indicator_vertices[5]; 
	indicator_edges[1] = indicator_vertices[2]; 
	indicator_edges[2] = indicator_vertices[5];
	indicator_edges[3] = indicator_vertices[0]; 
	indicator_edges[4] = indicator_vertices[5]; 
	indicator_edges[5] = indicator_vertices[1];
	indicator_edges[6] = indicator_vertices[5]; 
	indicator_edges[7] = indicator_vertices[3]; 
	// middle square
	indicator_edges[8] = indicator_vertices[2];
	indicator_edges[9] = indicator_vertices[3]; 
	indicator_edges[10] = indicator_vertices[3]; 
	indicator_edges[11] = indicator_vertices[1];
	indicator_edges[12] = indicator_vertices[1]; 
	indicator_edges[13] = indicator_vertices[0]; 
	indicator_edges[14] = indicator_vertices[0];
	indicator_edges[15] = indicator_vertices[2]; 
	// bottom four
	indicator_edges[16] = indicator_vertices[2]; 
	indicator_edges[17] = indicator_vertices[4];
	indicator_edges[18] = indicator_vertices[3]; 
	indicator_edges[19] = indicator_vertices[4]; 
	indicator_edges[20] = indicator_vertices[1]; 
	indicator_edges[21] = indicator_vertices[4]; 
	indicator_edges[22] = indicator_vertices[0]; 
	indicator_edges[23] = indicator_vertices[4];
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

void A1::moveActive(int x_pos, int z_pos) {
	active_cell.first += x_pos;
	active_cell.second += z_pos;

	if (active_cell.first < 0) active_cell.first = 0;
	if (active_cell.first > DIM-1) active_cell.first = DIM-1;
	if (active_cell.second < 0) active_cell.second = 0;
	if (active_cell.second > DIM-1) active_cell.second = DIM-1;

	if (shift_press) {
		cell_info[active_cell.first][active_cell.second].first = 
			cell_info[active_cell.first-x_pos][active_cell.second-z_pos].first;


		cell_info[active_cell.first][active_cell.second].second = 
			cell_info[active_cell.first-x_pos][active_cell.second-z_pos].second;
	}
}

void A1::reset() {
	active_cell.first = 0;
	active_cell.second = 0;

	current_col = 0;
	m_shape_size = 1.0f;
	m_shape_rotation = 0.0f;

	for (int i = 0; i < DIM; ++i) {
		for (int j = 0; j < DIM; ++j) {
			cell_info[i][j].first = 0;
			cell_info[i][j].second = 0;
		}
	}
}


//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if ( ImGui::IsMouseDown(0) ) {
			double x_distance = xPos - mouse_x_current_pos;
			m_shape_rotation += x_distance * 0.01;
		}
		mouse_x_current_pos = xPos;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {

	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.

	if (yOffSet > 0) {
		m_shape_size += yOffSet * 0.1f;
		m_shape_size = m_shape_size > 5 ? 5 : m_shape_size;
	}
	else {
		m_shape_size -= yOffSet * -0.1f;
		m_shape_size = m_shape_size < 0.1 ? 0.1f : m_shape_size;
	}

	eventHandled = true;

	return eventHandled;
}

//------------------------------------------------Scrolling down should make the grid smaller, scrolling up should make it bigger. The amount of scaling shoul----------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if (key == GLFW_KEY_R) {
			reset();
		}
		if (key == GLFW_KEY_SPACE) {
			cell_info[active_cell.first][active_cell.second].first += 1;
			cell_info[active_cell.first][active_cell.second].first =
			 cell_info[active_cell.first][active_cell.second].first > MAX_CUBE ? MAX_CUBE : cell_info[active_cell.first][active_cell.second].first;

			cell_info[active_cell.first][active_cell.second].second = current_col;

			if (symmetric_press) {
				cell_info[DIM-1-active_cell.first][active_cell.second].first = cell_info[active_cell.first][active_cell.second].first;
				cell_info[DIM-1-active_cell.first][active_cell.second].second = cell_info[active_cell.first][active_cell.second].second;
			}
		}
		if (key == GLFW_KEY_BACKSPACE) {
			cell_info[active_cell.first][active_cell.second].first -= 1;
			cell_info[active_cell.first][active_cell.second].first =
			 cell_info[active_cell.first][active_cell.second].first < 0 ? 0 : cell_info[active_cell.first][active_cell.second].first;

			if (symmetric_press) {
				cell_info[DIM-1-active_cell.first][active_cell.second].first = cell_info[active_cell.first][active_cell.second].first;
				cell_info[DIM-1-active_cell.first][active_cell.second].second = cell_info[active_cell.first][active_cell.second].second;
			}
		}
		if (key == GLFW_KEY_W) {
			m_shape_rotation_x += 0.05f;
		}
		if (key == GLFW_KEY_S) {
			m_shape_rotation_x -= 0.05f;
		}
		if (key == GLFW_KEY_C) {
			symmetric_press = 1;
		}
		if (key == GLFW_KEY_B) {
			border_press = !border_press;
		}
		if (key == GLFW_KEY_UP) {
			moveActive(0, -1);
		}
		if (key == GLFW_KEY_LEFT) {
			moveActive(-1, 0);
		}
		if (key == GLFW_KEY_RIGHT) {
			moveActive(1, 0);
		}
		if (key == GLFW_KEY_DOWN) {
			moveActive(0, 1);
		}
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			shift_press = 1;
		}
		eventHandled = true;
	}
	if ( action == GLFW_RELEASE ) {
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			shift_press = 0;
		}
		if (key == GLFW_KEY_C) {
			symmetric_press = 0;
		}
	}

	return eventHandled;
}
