#pragma once

#include "Material.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

class SceneNode {
public:
  SceneNode(const std::string & name);

  SceneNode(const SceneNode & other);

  virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
  const glm::mat4& get_transform() const;
  const glm::mat4& get_inverse() const;
    
  void set_transform(const glm::mat4& m);
    
  void add_child(SceneNode* child);
    
  void remove_child(SceneNode* child);

  void apply_effect_to_child();

  virtual glm::mat4 get_M();

  SceneNode * find_node_by_id(unsigned int id);

	//-- Transformations:
  void rotate(char axis, float angle);
  void scale(const glm::vec3& amount);
  void translate(const glm::vec3& amount);

	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

  void render(
    ShaderProgram & shader,
    glm::mat4 & view, 
    BatchInfoMap & batchInfoMap,
    const bool isPicking
  );

	bool isSelected;
    
  // Transformations
  glm::mat4 trans;
  glm::mat4 invtrans;
  glm::mat4 parent_trans;
  glm::mat4 trackball_mat;
  std::list<SceneNode*> children;

  SceneNode* parent;


	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;


private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
