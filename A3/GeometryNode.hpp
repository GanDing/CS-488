#pragma once

#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;

	void updateShaderUniforms(
    const ShaderProgram & shader,
    const glm::mat4 & viewMatrix,
    const bool isPicking
  );

  void geometryNodeRender(
  	ShaderProgram & shader,
    glm::mat4 & view, 
    BatchInfoMap & batchInfoMap,
    const bool isPicking
  );

};