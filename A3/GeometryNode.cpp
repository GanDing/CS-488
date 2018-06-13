#include "GeometryNode.hpp"

#include "cs488-framework/MathUtils.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::updateShaderUniforms(
		const ShaderProgram & shader,
		const glm::mat4 & viewMatrix,
		const bool isPicking
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		glm::mat4 modelView = viewMatrix * parent_trans * trans * trackball_mat;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		if (isPicking) {

			float r = float( m_nodeId       &0xff) / 255.0f;
			float g = float((m_nodeId >> 8) &0xff) / 255.0f;
			float b = float((m_nodeId >>16) &0xff) / 255.0f;

			location = shader.getUniformLocation("material.kd");
			glUniform3f( location, r, g, b );
			CHECK_GL_ERRORS;
		} else {
      
      //-- Set NormMatrix:
      location = shader.getUniformLocation("NormalMatrix");
      mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
      glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
      CHECK_GL_ERRORS;

      //-- Set Material values:
      location = shader.getUniformLocation("material.kd");
      vec3 kd = material.kd;

      if (isSelected) {
      	kd = {1, 1, 0};
    	}

      glUniform3fv(location, 1, value_ptr(kd));
      CHECK_GL_ERRORS;
      location = shader.getUniformLocation("material.ks");
      vec3 ks = material.ks;
      glUniform3fv(location, 1, value_ptr(ks));
      CHECK_GL_ERRORS;
      location = shader.getUniformLocation("material.shininess");
      glUniform1f(location, material.shininess);
      CHECK_GL_ERRORS;			
		}
	}
	shader.disable();
}

void GeometryNode::geometryNodeRender(
	ShaderProgram & shader,
  mat4 & view, 
  BatchInfoMap & batchInfoMap,
  const bool isPicking
	) {
	updateShaderUniforms(shader, view, isPicking);

  // Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
  BatchInfo batchInfo = batchInfoMap[meshId];

  //-- Now render the mesh:
  shader.enable();
  glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
  shader.disable();
}