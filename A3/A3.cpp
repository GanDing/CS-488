#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

  reset();

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

void A3::reset() {
	for (int i = 0; i < 4; ++i) {
		option[i] = false;
	}

  rotating = false;
  do_picking = false;

  mouse_x_current_pos = 0;
  mouse_y_current_pos = 0;

  joint_x_angle = 0;
  joint_y_angle = 0;

  translate_history = glm::vec3();
  rotate_history = glm::mat4();

  current_select_node.clear();
}

void A3::resetOrientation() {
  m_rootNode->trackball_mat = inverse(rotate_history) * m_rootNode->trackball_mat;
  m_rootNode->apply_effect_to_child();
  rotate_history = mat4();
}

void A3::resetTranslation() {
  m_rootNode->translate(-translate_history);
  translate_history = vec3();
}

void A3::resetJoint() {
  while (undo_stack.size() !=0) {
    undo();
  }
  redo_stack.clear();
  current_select_node.clear();
  m_rootNode->reset_select();
}

void A3::resetAll() {
  resetOrientation();
  resetTranslation();
  resetJoint();
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

    location = m_shader.getUniformLocation("picking");
    glUniform1i( location, do_picking ? 1 : 0 );
    CHECK_GL_ERRORS;

		//-- Set LightSource uniform for the scene:
    if( !do_picking ) {
      location = m_shader.getUniformLocation("light.position");
      glUniform3fv(location, 1, value_ptr(m_light.position));
      location = m_shader.getUniformLocation("light.rgbIntensity");
      glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
      CHECK_GL_ERRORS;

      location = m_shader.getUniformLocation("ambientIntensity");
      vec3 ambientIntensity(0.05f);
      glUniform3fv(location, 1, value_ptr(ambientIntensity));
      CHECK_GL_ERRORS;
    }
		// {
		// 	location = m_shader.getUniformLocation("light.position");
		// 	glUniform3fv(location, 1, value_ptr(m_light.position));
		// 	location = m_shader.getUniformLocation("light.rgbIntensity");
		// 	glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
		// 	CHECK_GL_ERRORS;
		// }

		// //-- Set background light ambient intensity
		// {
		// 	location = m_shader.getUniformLocation("ambientIntensity");
		// 	vec3 ambientIntensity(0.05f);
		// 	glUniform3fv(location, 1, value_ptr(ambientIntensity));
		// 	CHECK_GL_ERRORS;
		// }
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

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

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Application")) {
			if (ImGui::MenuItem("Reset Position", "I")) {
			}
			if (ImGui::MenuItem("Reset Orientation", "O")) {
			}
			if (ImGui::MenuItem("Reset Joints", "N")) {
			}
			if (ImGui::MenuItem("Reset All", "A")) {
			}
			if (ImGui::MenuItem("Quit", "Q")) {
				glfwSetWindowShouldClose(m_window, GL_TRUE);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "U")) {
			}
			if (ImGui::MenuItem("Redo", "R")) {
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options")) {
			(ImGui::MenuItem("Circle", "C", &option[0], true));
			(ImGui::MenuItem("Z-buffer", "Z", &option[1], true));
			(ImGui::MenuItem("Backface", "B", &option[2], true));
			(ImGui::MenuItem("Frontface", "F", &option[3], true));
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

  ImGui::PushID( 0 );
  ImGui::Text( "Position/Orientation(P)" );
  ImGui::SameLine();
  if( ImGui::RadioButton( "##Mode", &current_mode, 0 ) ) {
  }
  ImGui::PopID();

  ImGui::PushID( 1 );
  ImGui::Text( "Joints(J)  \t\t\t" );
  ImGui::SameLine();
  if( ImGui::RadioButton( "##Mode", &current_mode, 1 ) ) {
  }
  ImGui::PopID();

  ImGui::Text( "You can undo: %lu time(s)", undo_stack.size() );
  ImGui::Text( "You can redo: %lu time(s)", redo_stack.size() );

	ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
		CHECK_GL_ERRORS;

	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {
	if (!option[1]) {			// z-buffer test
		glEnable( GL_DEPTH_TEST );
	}

	if (option[2] || option[3]) {
		glEnable( GL_CULL_FACE );
		if (!option[2]) {		// cull back face
			glCullFace( GL_FRONT );
		} else if (!option[3]) {	// cull front face
			glCullFace( GL_BACK );
		} else {
			glCullFace( GL_FRONT_AND_BACK );
		}
	}
	renderSceneGraph(*m_rootNode);


	glDisable( GL_DEPTH_TEST );
	if (option[0]) {			// circle test
		renderArcCircle();
	}
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph( SceneNode & root ) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.

	root.render(m_shader, m_view, m_batchInfoMap, do_picking);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void A3::undo() {
	if (undo_stack.size() == 0) {
		return;
	}
	std::list<std::pair<JointNode *, glm::vec3>> latest_command = undo_stack.back();
	undo_stack.pop_back();

  for (pair<JointNode *, glm::vec3> command : latest_command) {
    JointNode * joint_node = command.first;
    vec3 rotations = command.second;
    joint_node->start_rotate(-rotations);
    vec3 temp = vec3();
    joint_node->update_rotate_angle(temp);
  }
  redo_stack.push_back(latest_command);
}

void A3::redo() {
  if (redo_stack.size() == 0) {
    return;
  }
  std::list<std::pair<JointNode *, glm::vec3>> latest_command = redo_stack.back();
  redo_stack.pop_back();

  for (pair<JointNode *, glm::vec3> command : latest_command) {
    JointNode * joint_node = command.first;
    vec3 rotations = command.second;
    joint_node->start_rotate(rotations);
    vec3 temp = vec3();
    joint_node->update_rotate_angle(temp);
  }
    undo_stack.push_back(latest_command);
}
//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		double x_distance = xPos - mouse_x_current_pos;
		double y_distance = yPos - mouse_y_current_pos;
		eventHandled = true;
		if (current_mode == 0) {     // translate
      if (ImGui::IsMouseDown(0)) {
        translate_history += vec3(0.001 * x_distance,-0.001 * y_distance,0);
        m_rootNode->translate(vec3(0.001 * x_distance,-0.001 * y_distance,0));
      }
      if(ImGui::IsMouseDown(2)) { 
        translate_history += vec3(0,0,0.01 * y_distance);
        m_rootNode->translate(vec3(0,0,0.01 * y_distance));
      }
      if(ImGui::IsMouseDown(1)) { 
        double diameter = m_framebufferWidth < m_framebufferHeight ? float(m_framebufferWidth) : float(m_framebufferHeight);
        double radius = diameter / 2;
        double center_x = float(m_framebufferWidth) / 2;
        double center_y = float(m_framebufferHeight) / 2;

        vec3 v = vCalcRotVec(xPos - center_x, center_y - yPos, 
                             mouse_x_current_pos - center_x, center_y - mouse_y_current_pos, radius);
        mat4 rotate_matrix = mat4();
        vAxisRotMatrix(v.x, v.y, v.z, rotate_matrix);

        rotate_history = rotate_matrix * rotate_history;
        m_rootNode->trackball_mat = rotate_matrix * m_rootNode->trackball_mat;
        m_rootNode->apply_effect_to_child();
      }
		}
    if (current_mode == 1) {    // joint
      if(ImGui::IsMouseDown(2)) { 
        rotating = true;
        double angle = 0.2 * y_distance;
        joint_x_angle = joint_x_angle + angle;
      }
      if(ImGui::IsMouseDown(1)) {
        rotating = true;
        double angle = 0.2 * x_distance;
        joint_y_angle = joint_y_angle + angle;
      }
      for(SceneNode * node : current_select_node){
        JointNode * joint_node = (JointNode *)node->parent;
        joint_node->start_rotate(vec3(joint_x_angle, joint_y_angle, 0));
      }
      eventHandled = true;
    }
	}

	mouse_x_current_pos = xPos;
	mouse_y_current_pos = yPos;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);
  if( current_mode == 1 ){ // joint
    if ( button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS && !rotating) {
      double xpos, ypos;
      glfwGetCursorPos( m_window, &xpos, &ypos );

      do_picking = true;

      uploadCommonSceneUniforms();
      glClearColor(1.0, 1.0, 1.0, 1.0 );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      glClearColor(0.35, 0.35, 0.35, 1.0);

      draw();

      // I don't know if these are really necessary anymore.
      // glFlush();
      // glFinish();

      CHECK_GL_ERRORS;

      // Ugly -- FB coordinates might be different than Window coordinates
      // (e.g., on a retina display).  Must compensate.
      xpos *= double(m_framebufferWidth) / double(m_windowWidth);
      // WTF, don't know why I have to measure y relative to the bottom of
      // the window in this case.
      ypos = m_windowHeight - ypos;
      ypos *= double(m_framebufferHeight) / double(m_windowHeight);

      GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
      // A bit ugly -- don't want to swap the just-drawn false colours
      // to the screen, so read from the back buffer.
      glReadBuffer( GL_BACK );
      // Actually read the pixel at the mouse location.
      glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
      CHECK_GL_ERRORS;

      // Reassemble the object ID.
      unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

      do_picking = false;
      CHECK_GL_ERRORS;

      SceneNode * select_node = m_rootNode->find_node_by_id(what);
      if (select_node == NULL) {
      	return false;
      } else {
        if( select_node->isSelected ){
          current_select_node.push_back(select_node);
        } else {
          current_select_node.remove(select_node);
        }
      }
      cout << *select_node << endl;
    }
    if ( actions == GLFW_RELEASE ) {
      if ( rotating ) {
        rotating = false;
        // reset the rotate angle
        joint_x_angle = 0;
        joint_y_angle = 0;
        std::list<std::pair<JointNode *, glm::vec3>> command_list;
        for( SceneNode * node : current_select_node ){
          JointNode * joint = (JointNode *)node->parent;
          vec3 final_angle = vec3();
          joint->update_rotate_angle(final_angle);

          if (final_angle != vec3()) {  // there is no move
            pair<JointNode *, glm::vec3> command;
            command.first = joint;
            command.second = vec3(final_angle);
            command_list.push_back(command);
          }
          
        } // for
        if(command_list.size() > 0) {
          undo_stack.push_back(command_list);
          redo_stack.clear();
        }
        eventHandled = true;
      } // if
    }    
  }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
		}
		if ( key == GLFW_KEY_Q ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
    if( key == GLFW_KEY_I ) {
      resetTranslation();
    }
    if( key == GLFW_KEY_O ) {
      resetOrientation();
    }
    if( key == GLFW_KEY_N ) {
      resetJoint();
    }
    if( key == GLFW_KEY_A ) {
      resetAll();
    }
    if( key == GLFW_KEY_C ) {
      option[0] = !option[0];
    }
    if( key == GLFW_KEY_Z ) {
      option[1] = !option[1];
    }
    if( key == GLFW_KEY_B ) {
      option[2] = !option[2];
    }
    if( key == GLFW_KEY_F ) {
      option[3] = !option[3];
    }
    if( key == GLFW_KEY_U ) {
    	undo();
    }
    if( key == GLFW_KEY_R ) {
      cout << "redo" << endl;
    	redo();
    }
    if( key == GLFW_KEY_P ) {
      current_mode = 0;
    }
    if( key == GLFW_KEY_J ) {
      current_mode = 1;
    }
    eventHandled = true;
	}

	return eventHandled;
}






//----------------------------------------------------------------------------------------
/*
 * Trackball Function
 */


/*******************************************************
 * 
 * void vCalcRotVec(float fNewX, float fNewY, 
 *                  float fOldX, float fOldY,
 *                  float fDiameter,
 *                  float *fVecX, float *fVecY, float *fVecZ);
 *
 *    Calculates a rotation vector based on mouse motion over
 *    a virtual trackball.
 *
 *    The fNew and fOld mouse positions
 *    should be in 'trackball' space. That is, they have been
 *    transformed into a coordinate system centered at the middle
 *    of the trackball. fNew, fOld, and fDiameter must all be specified
 *    in the same units (pixels for example).
 *
 * Parameters: fNewX, fNewY - New mouse position in trackball space.
 *                            This is the second point along direction
 *                            of rotation.
 *             fOldX, fOldY - Old mouse position in trackball space.
 *                            This is the first point along direction
 *                            of rotation.
 *             fDiameter - Diameter of the trackball. This should
 *                         be specified in the same units as fNew and fOld.
 *                         (ie, usually pixels if fNew and fOld are transformed
 *                         mouse positions)
 *             fVec - The output rotation vector. The length of the vector
 *                    is proportional to the angle of rotation.
 *
 *******************************************************/
glm::vec3 A3::vCalcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter
                 ) {
   long  nXOrigin, nYOrigin;
   float fNewVecX, fNewVecY, fNewVecZ,        /* Vector corresponding to new mouse location */
         fOldVecX, fOldVecY, fOldVecZ,        /* Vector corresponding to old mouse location */
         fLength;

    glm::vec3 result;
   /* Vector pointing from center of virtual trackball to
    * new mouse position
    */
   fNewVecX    = fNewX * 2.0 / fDiameter;
   fNewVecY    = fNewY * 2.0 / fDiameter;
   fNewVecZ    = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fNewVecZ < 0.0) {
      fLength = sqrt(1.0 - fNewVecZ);
      fNewVecZ  = 0.0;
      fNewVecX /= fLength;
      fNewVecY /= fLength;
   } else {
      fNewVecZ = sqrt(fNewVecZ);
   }

   /* Vector pointing from center of virtual trackball to
    * old mouse position
    */
   fOldVecX    = fOldX * 2.0 / fDiameter;
   fOldVecY    = fOldY * 2.0 / fDiameter;
   fOldVecZ    = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);
 
   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fOldVecZ < 0.0) {
      fLength = sqrt(1.0 - fOldVecZ);
      fOldVecZ  = 0.0;
      fOldVecX /= fLength;
      fOldVecY /= fLength;
   } else {
      fOldVecZ = sqrt(fOldVecZ);
   }

   /* Generate rotation vector by calculating cross product:
    * 
    * fOldVec x fNewVec.
    * 
    * The rotation vector is the axis of rotation
    * and is non-unit length since the length of a crossproduct
    * is related to the angle between fOldVec and fNewVec which we need
    * in order to perform the rotation.
    */
   float fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
   float fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
   float fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;

   result = glm::vec3(fVecX, fVecY, fVecZ);
   return result;
}

/*******************************************************
 * void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix mNewMat)
 *    
 *    Calculate the rotation matrix for rotation about an arbitrary axis.
 *    
 *    The axis of rotation is specified by (fVecX,fVecY,fVecZ). The length
 *    of the vector is the amount to rotate by.
 *
 * Parameters: fVecX,fVecY,fVecZ - Axis of rotation
 *             mNewMat - Output matrix of rotation in column-major format
 *                       (ie, translation components are in column 3 on rows
 *                       0,1, and 2).
 *
 *******************************************************/
void A3::vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, glm::mat4 &mNewMat) {
    float fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;
    /* Find the length of the vector which is the angle of rotation
     * (in radians)
     */
    fRadians = sqrt(fVecX * fVecX + fVecY * fVecY + fVecZ * fVecZ);

    /* If the vector has zero length - return the identity matrix */
    if (fRadians > -0.000001 && fRadians < 0.000001) {
        return;
    }

    /* Normalize the rotation vector now in preparation for making
     * rotation matrix. 
     */
    fInvLength = 1 / fRadians;
    fNewVecX   = fVecX * fInvLength;
    fNewVecY   = fVecY * fInvLength;
    fNewVecZ   = fVecZ * fInvLength;

    /* Create the arbitrary axis rotation matrix */
    double dSinAlpha = sin(fRadians);
    double dCosAlpha = cos(fRadians);
    double dT = 1 - dCosAlpha;

    mNewMat[0][0] = dCosAlpha + fNewVecX*fNewVecX*dT;
    mNewMat[0][1] = fNewVecX*fNewVecY*dT + fNewVecZ*dSinAlpha;
    mNewMat[0][2] = fNewVecX*fNewVecZ*dT - fNewVecY*dSinAlpha;
    mNewMat[0][3] = 0;

    mNewMat[1][0] = fNewVecX*fNewVecY*dT - dSinAlpha*fNewVecZ;
    mNewMat[1][1] = dCosAlpha + fNewVecY*fNewVecY*dT;
    mNewMat[1][2] = fNewVecY*fNewVecZ*dT + dSinAlpha*fNewVecX;
    mNewMat[1][3] = 0;

    mNewMat[2][0] = fNewVecZ*fNewVecX*dT + dSinAlpha*fNewVecY;
    mNewMat[2][1] = fNewVecZ*fNewVecY*dT - dSinAlpha*fNewVecX;
    mNewMat[2][2] = dCosAlpha + fNewVecZ*fNewVecZ*dT;
    mNewMat[2][3] = 0;

    mNewMat[3][0] = 0;
    mNewMat[3][1] = 0;
    mNewMat[3][2] = 0;
    mNewMat[3][3] = 1;
}

