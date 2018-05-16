#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <stdlib.h> 
#include <stdio.h> 
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;

Cell::Cell( GLint nx, GLint nz) { 
    
  this->x = nx;
  this->z = nz;
  this->height = 0;
  this->color = 0;
  this->vertices = new GLfloat[108];

}

Cell::~Cell()
{
  delete[] this->vertices;	
}

GLfloat * Cell::genV(GLint xpos, GLint zpos,GLint h) {
   
  GLfloat * ver = new GLfloat[108] {	  
  
  //v0 xpos + 1.0f,1.0f + h ,zpos + 1.0f,
  //v1 xpos + 0.0f,1.0f + h ,zpos + 1.0f,
  //v2 xpos + 0.0f,0.0f     ,zpos + 1.0f,
  //v3 xpos + 1.0f,0.0f     ,zpos + 1.0f,
  //v4 xpos + 1.0f,0.0f     ,zpos + 0.0f,
  //v5 xpos + 1.0f,1.0f + h ,zpos + 0.0f,
  //v6 xpos + 0.0f,1.0f + h ,zpos + 0.0f,
  //v7 xpos + 0.0f,0.0f     ,zpos + 0.0f,

    
  // front face
    xpos + 1.0f,0.0f + h,zpos + 1.0f, 
   xpos + 0.0f,0.0f + h,zpos + 1.0f,
   xpos + 0.0f,0.0f,    zpos + 1.0f,
   xpos + 0.0f,0.0f,    zpos + 1.0f,
   xpos + 1.0f,0.0f,    zpos + 1.0f,
   xpos + 1.0f,0.0f + h,zpos + 1.0f,	  

   // right face
   xpos + 1.0f,0.0f + h,zpos + 1.0f,
   xpos + 1.0f,0.0f    ,zpos + 1.0f,
   xpos + 1.0f,0.0f    ,zpos + 0.0f,
   xpos + 1.0f,0.0f    ,zpos + 0.0f,
   xpos + 1.0f,0.0f + h,zpos + 0.0f,
   xpos + 1.0f,0.0f + h,zpos + 1.0f,

   // back face 
   xpos + 0.0f,0.0f + h ,zpos + 0.0f,
   xpos + 1.0f,0.0f + h ,zpos + 0.0f,
   xpos + 1.0f,0.0f     ,zpos + 0.0f,
   xpos + 1.0f,0.0f     ,zpos + 0.0f,
   xpos + 0.0f,0.0f     ,zpos + 0.0f,
   xpos + 0.0f,0.0f + h ,zpos + 0.0f,
 
   // left face
   xpos + 0.0f,0.0f + h ,zpos + 1.0f,
   xpos + 0.0f,0.0f + h ,zpos + 0.0f,
   xpos + 0.0f,0.0f     ,zpos + 0.0f,
   xpos + 0.0f,0.0f     ,zpos + 0.0f,
   xpos + 0.0f,0.0f     ,zpos + 1.0f,
   xpos + 0.0f,0.0f + h ,zpos + 1.0f,

   //top face 
   
   xpos + 0.0f,0.0f + h ,zpos + 0.0f,
   xpos + 1.0f,0.0f + h ,zpos + 0.0f,
   xpos + 1.0f,0.0f + h ,zpos + 1.0f,
   xpos + 1.0f,0.0f + h ,zpos + 1.0f,
   xpos + 0.0f,0.0f + h ,zpos + 1.0f,
   xpos + 0.0f,0.0f + h ,zpos + 0.0f,

   // bottom face
   xpos + 0.0f,0.0f     ,zpos + 0.0f,
   xpos + 1.0f,0.0f     ,zpos + 0.0f,
   xpos + 1.0f,0.0f     ,zpos + 1.0f,
   xpos + 1.0f,0.0f     ,zpos + 1.0f,
   xpos + 0.0f,0.0f     ,zpos + 1.0f,
   xpos + 0.0f,0.0f     ,zpos + 0.0f,


  };



  return ver;
  
}





//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 )
{
       srand((unsigned)time(0));	
       for (int l = 0; l < 8; l += 1) {
	 colour[l] = new float[3];      
         colour[l][0] = ((float) rand()) / (float) RAND_MAX;; 		
	 colour[l][1] = ((float) rand()) / (float) RAND_MAX;;
         colour[l][2] = ((float) rand()) / (float) RAND_MAX;;	 
	}
	
	for (int i = 0; i < 16; i += 1) {
          for (int j = 0; j < 16; j += 1) {
             GLint xpos = i;
	     GLint zpos  = j;
             Cells[i][j] = new Cell(xpos,zpos);		  
	  }
	}

	radionum = 0;
	curx = 0;
	curz = 0;
	scale = 1.0f;
	rot = 0.0f;
	trans = vec2(0.0f);
	lshift = false;
	rshift = false;
	m_mouseButtonActive = false;
        clickx = 0;
	clicky = 0;
	mx = 0;
	my = 0;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
        for (int i = 0; i < 16; i += 1) {
          for (int j = 0; j < 16; j += 1) {
             delete Cells[i][j];             
          }       
        }
        
	if (indicator != NULL) {
	delete[] indicator;
	}
	for (int l = 0; l < 8; l += 1) {
	  delete[] colour[l]; 
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
//----------------------------------------------------------------------------------------
/*void A1::uploadUniformsToShader()
{
        m_shader.enable();
                // Query the shader program for the location number of the uniform named "color".
                GLint uniformLocation_colour = m_shader.getUniformLocation("colour");

                // Set the uniform's value.
                glUniform3f(uniformLocation_colour, m_shape_color.r, m_shape_color.g,
                                m_shape_color.b);

                vec3 z_axis(0.0f,0.0f,1.0f);
                mat4 transform = glm::translate(mat4(), vec3(trans, 0.0f));
                transform *= glm::scale(mat4(), vec3(scale));
                transform *= glm::rotate(mat4(),rot, z_axis);

               glUniformMatrix4fv(M_uni, 1, GL_FALSE,
                                glm::value_ptr(transform));

        m_shader.disable();

        CHECK_GL_ERRORS;
}*/
//---------------------------------------------------------------------------------
void A1::copyCell(int ox, int oz, int nx, int nz) {

  if (Cells[nx][nz] -> vertices != NULL) {
    delete[] Cells[nx][nz] -> vertices;
  }
  Cells[nx][nz] -> vertices = new GLfloat[108];
  for (int i = 0; i < 108; i += 1) {
    (Cells[nx][nz] -> vertices)[i] = (Cells[ox][oz] -> vertices)[i]; 	  
  }  
  Cells[nx][nz] -> height = Cells[ox][oz] -> height;
  Cells[nx][nz] -> color = Cells[ox][oz] -> color; 


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
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
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



		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		for (int idx = 0; idx < 8; idx += 1) {
		  ImGui::PushID( idx );
		  ImGui::ColorEdit3( "##Colour", colour[idx] );
		  ImGui::SameLine();
		  if( ImGui::RadioButton( "##Col", &current_col, idx ) ) {
			// Select this colour.
		       cout << "cur col: " << current_col << endl;
                       Cells[curx][curz] -> color = current_col;
		  }
		  ImGui::PopID();
		}

               /* ImGui::PushID( 1 );
                ImGui::ColorEdit3( "##Colour", colour[1] );
                ImGui::SameLine();
                if( ImGui::RadioButton( "##Col", &current_col, 1 ) ) {
                        // Select this colour.
                }
                ImGui::PopID();*/


                /*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}*/

                //ImGui::SliderFloat("Rotation", &rot, 0.0f, 6.27f);

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

void A1::drawCube(GLint px, GLint pz) {
     
     int color = Cells[px][pz] -> color;
     GLfloat th = Cells[px][pz] -> height;
     GLfloat* vertices = Cells[px][pz] -> genV(px,pz, th);
     GLuint cube_vao;
     GLuint cube_vbo;

     glGenVertexArrays( 1, &cube_vao);
     glBindVertexArray( cube_vao );

     glGenBuffers( 1, &cube_vbo );
     glBindBuffer( GL_ARRAY_BUFFER, cube_vbo );
     glBufferData( GL_ARRAY_BUFFER, 3*36 * sizeof(float),
     vertices, GL_STATIC_DRAW);

     GLint posAttrib = m_shader.getAttribLocation( "position" );
     glEnableVertexAttribArray( posAttrib );
     glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

     glBindVertexArray( cube_vao );
     glUniform3f( col_uni, colour[color][0], colour[color][1], colour[color][2] );    
     glDrawArrays( GL_TRIANGLES , 0, 36);
}


GLfloat* A1::genIndicator(GLint px, GLint pz) {
  
  GLint xpos = px;
  GLint zpos = pz;
  GLint h = Cells[curx][curz] -> height + 1;
  
  GLfloat * ind = new GLfloat[54] {

       //v0 xpos + 1.0f,1.0f + h ,zpos + 1.0f,
       //v1 xpos + 0.0f,1.0f + h ,zpos + 1.0f,
       //v2 xpos + 0.0f,0.0f     ,zpos + 1.0f,
       //v3 xpos + 1.0f,0.0f     ,zpos + 1.0f,
       //v4 xpos + 1.0f,0.0f     ,zpos + 0.0f,
       //v5 xpos + 1.0f,1.0f + h ,zpos + 0.0f,
       //v6 xpos + 0.0f,1.0f + h ,zpos + 0.0f,
       //v7 xpos + 0.0f,0.0f     ,zpos + 0.0f,	  
       //top face 
       xpos + 0.0f,0.0f + h ,zpos + 0.0f,
       xpos + 1.0f,0.0f + h ,zpos + 0.0f,
       xpos + 1.0f,0.0f + h ,zpos + 1.0f,
       xpos + 1.0f,0.0f + h ,zpos + 1.0f,
       xpos + 0.0f,0.0f + h ,zpos + 1.0f,
       xpos + 0.0f,0.0f + h ,zpos + 0.0f,

       // botter center  xpos + 0.5f, 0.0f + h - 1.0f, zpos + 0.5f,
       // front face
       xpos + 1.0f,0.0f + h ,zpos + 1.0f,
       xpos + 0.0f,0.0f + h ,zpos + 1.0f,
       xpos + 0.5f, 0.0f + h - 1.0f, zpos + 0.5f,
       // back face
       xpos + 1.0f,0.0f + h ,zpos + 0.0f,
       xpos + 0.0f,0.0f + h ,zpos + 0.0f,
       xpos + 0.5f, 0.0f + h - 1.0f, zpos + 0.5f,

       // left face
       xpos + 0.0f,0.0f + h ,zpos + 1.0f,
       xpos + 0.0f,0.0f + h ,zpos + 0.0f,
       xpos + 0.5f, 0.0f + h - 1.0f, zpos + 0.5f,
  
       // right face
       xpos + 0.5f, 0.0f + h - 1.0f, zpos + 0.5f,
       xpos + 1.0f,0.0f + h ,zpos + 0.0f,
       xpos + 1.0f,0.0f + h ,zpos + 1.0f
  };	  

  return ind;
}




void A1::drawIndicator(GLint px, GLint pz) {
  
     if (indicator != NULL) {
	 delete[] indicator;
     }
     indicator = genIndicator(px,pz);     
     GLuint ind_vao;
     GLuint ind_vbo;

     glGenVertexArrays( 1, &ind_vao);
     glBindVertexArray( ind_vao );

     glGenBuffers( 1, &ind_vbo );
     glBindBuffer( GL_ARRAY_BUFFER, ind_vbo );
     glBufferData( GL_ARRAY_BUFFER, 3*18 * sizeof(float),
     indicator, GL_STATIC_DRAW);

     GLint posAttrib = m_shader.getAttribLocation( "position" );
     glEnableVertexAttribArray( posAttrib );
     glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

     glBindVertexArray( ind_vao );
     glUniform3f( col_uni, 0, 0, 0 );
     glDrawArrays( GL_TRIANGLES , 0, 18);

}


void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
        vec3 z_axis(0.0f,1.0f,0.0f);
	W = glm::rotate(mat4(),rot, z_axis);
	W *= glm::scale(mat4(), vec3(scale));
	W *= glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
        
      

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );
  
		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
 
		for (int i = 0; i < 16; i += 1) {
	          for (int j = 0; j < 16; j += 1) {
	            GLint tpx = i;
                    GLint tpz = j;
                    if (Cells[i][j] -> height != 0) {		    
                      drawCube(tpx,tpz);
		    }
	          }  
			
		}		
		
		
	

		// Highlight the active square.
		drawIndicator(curx,curz);

	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}
//----------------------------------------------------------------------------------------
void A1::reset() {

	for (int i = 0; i < 16; i += 1) {
          for (int j = 0; j < 16; j += 1) {
             GLint xpos = i;
             GLint zpos  = j;
	     delete Cells[i][j];
             Cells[i][j] = new Cell(xpos,zpos);
          }
        }

        radionum = 0;
        curx = 0;
        curz = 0;
        scale = 1.0f;
        rot = 0.0f;
        trans = vec2(0.0f);
        lshift = false;
        rshift = false;
        m_mouseButtonActive = false;
        clickx = 0;
        clicky = 0;
        mx = 0;
        my = 0;
	current_col = 0;
       
         
}
//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

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
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
		mx = (2.0f * xPos) / m_windowWidth - 1.0f;
		my = 1.0f - ( (2.0f * yPos) / m_windowHeight);
		if (m_mouseButtonActive == true) {
		  	
		   rot = (mx - clickx) * 0.9;
		}
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
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
            if (actions == GLFW_PRESS) {
                  if (!ImGui::IsMouseHoveringAnyWindow()) {
                        m_mouseButtonActive = true;
                        clickx = mx;
		        clicky = my;	
                }
            }

            if (actions == GLFW_RELEASE) {
                m_mouseButtonActive = false;
		clickx = 0;
		clicky = 0;
            }  

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
	if (scale + yOffSet/50 > 10.0f) {
            scale = 10.0f;
	} else if (scale + yOffSet/50 < 0.1f) {
            scale = 0.1f;
	} else {
            scale += yOffSet/50;
	}
        eventHandled = true;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
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
		// Respond to some key events.
            if (key == GLFW_KEY_SPACE) {
              
              Cells[curx][curz] -> height +=1 ;
              eventHandled = true;
            }

	    if (key == GLFW_KEY_BACKSPACE ) {
              if (Cells[curx][curz] -> height >= 1) {
               Cells[curx][curz] -> height -= 1 ; 
	      } else {
	       Cells[curx][curz] -> height = 0;
	      }
              eventHandled = true;
             }

	    if (key == GLFW_KEY_UP) {
	      int oz = curz;
              int ox = curx;	      
	      curz -= 1;
	      if (curz < 0) {
                curz = 0;
	      } 
              if (curz > 15) {
		curz = 15;
	      }
	      if (rshift == true || lshift == true) {
		copyCell(ox,oz,curx,curz);       
	      }
	      Cells[curx][curz] -> color = current_col;
	      eventHandled = true;
	    }

           if (key == GLFW_KEY_DOWN) {	      
              int oz = curz;
              int ox = curx;
	      curz += 1;
              if (curz < 0) {
                curz = 0;
              }
              if (curz > 15) {
                curz = 15;
              }
              if (rshift == true || lshift == true) {
                copyCell(ox,oz,curx,curz);
              }	      
	      Cells[curx][curz] -> color = current_col;
	      eventHandled = true;
            }
           if (key == GLFW_KEY_LEFT) {
              int oz = curz;
              int ox = curx;
              curx -= 1;
              if (curx < 0) {
                curx = 0;
              }
              if (curx > 15) {
                curx = 15;
              }
              if (rshift == true || lshift == true) {
                copyCell(ox,oz,curx,curz);
              }
              Cells[curx][curz] -> color = current_col;
              eventHandled = true;
            }
           if (key == GLFW_KEY_RIGHT) {
              int oz = curz;
              int ox = curx;		   
              curx+= 1;
              if (curx < 0) {
                curx = 0;
              }
              if (curx > 15) {
                curx = 15;
              }
              if (rshift == true || lshift == true) {
                copyCell(ox,oz,curx,curz);
              }
              Cells[curx][curz] -> color = current_col;
              eventHandled = true;
            }

           if (key == GLFW_KEY_LEFT_SHIFT) {
              
              lshift = true;		   
              eventHandled = true;
            }

           if (key == GLFW_KEY_RIGHT_SHIFT ) {
              rshift = true;
              eventHandled = true;
            }

	   if (key == GLFW_KEY_Q) {
              glfwSetWindowShouldClose(m_window, GL_TRUE);
              eventHandled = true;
	   }

           if (key == GLFW_KEY_R) {
              reset();
              eventHandled = true;
           }
	   

        }

        if (action == GLFW_RELEASE) {
           if (key == GLFW_KEY_LEFT_SHIFT) {

              lshift = false;
              eventHandled = true;
            }
           if (key == GLFW_KEY_RIGHT_SHIFT) {

              rshift = false;
              eventHandled = true;
            }	
	}

	return eventHandled;
}
