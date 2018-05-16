#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "grid.hpp"

class Cell
{
  public:

    GLfloat* vertices;	  
    GLint x;
    GLint z;
    GLint height;
    int color;

    Cell(GLint x, GLint z);
    virtual ~Cell();
    GLfloat* genV(GLint xpos, GLint zpos,GLint h);
    void inH();
    void deH();

};

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();
	void drawCube(GLint px, GLint pz);
	GLfloat* genIndicator(GLint px, GLint pz);
        void drawIndicator(GLint px, GLint pz);

protected:
	virtual void init() override;
	//void uploadUniformsToShader();
	void copyCell(int ox, int oz, int nx, int nz);
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	void reset();
	virtual void cleanup() override;
         
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

private:
	void initGrid();

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	float* colour[8];
	int current_col;
        int radionum;

        Cell* Cells[16][16];
	int curx;
	int curz;
	GLfloat * indicator;
        float scale;
	float rot;
        glm::vec2 trans;
	bool lshift;
	bool rshift;
        bool m_mouseButtonActive;
        double clickx;
        double clicky;
        double mx;
        double my;	
};
