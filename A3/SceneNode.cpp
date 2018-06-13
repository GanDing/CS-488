#include "SceneNode.hpp"
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


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
  parent_trans(mat4()),
  trackball_mat(mat4()),
	isSelected(false),
  parent(NULL),
	m_nodeId(nodeInstanceCount++)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  invtrans(other.invtrans),
    parent_trans(other.parent_trans),
    trackball_mat(other.trackball_mat)
{
	for(SceneNode * child : other.children) {
		this->children.push_front(new SceneNode(*child));
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	invtrans = m;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	child->parent = this;
	child->parent_trans = get_M();
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::apply_effect_to_child() {
	for(SceneNode * child : children) {
		child->parent_trans = get_M();
		child->apply_effect_to_child();
	}
}

void SceneNode::reset_select() {
	isSelected = false;
	for (SceneNode * child : children) {
		child->reset_select();
	}
}

SceneNode * SceneNode::find_node_by_id(unsigned int id) {
	if (id == m_nodeId) {
		if (parent->m_nodeType == NodeType::JointNode) {
			isSelected = !isSelected;
			return this;
		}
		return NULL;
	}
	for (SceneNode * child : children) {
		SceneNode * result = child->find_node_by_id(id);
		if( result ) {
			return result;
		}
	}
	return NULL;
}

glm::mat4 SceneNode::get_M(){
	return parent_trans * trans * trackball_mat;
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	trans = rot_matrix * trans;
	apply_effect_to_child();
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	trans = glm::scale(amount) * trans;
	apply_effect_to_child();
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	trans = glm::translate(amount) * trans;
	apply_effect_to_child();
}


//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;
	os << "]";

	return os;
}

void SceneNode::render(
  ShaderProgram & shader, 
  glm::mat4 & view, 
  BatchInfoMap & batchInfoMap,
  const bool isPicking
) {
  if (m_nodeType == NodeType::GeometryNode) {
    GeometryNode * geometryNode = static_cast<GeometryNode *>(this);
    geometryNode->geometryNodeRender(shader, view, batchInfoMap, isPicking);
  }
  for (SceneNode * child : children) {
    child->render(shader, view, batchInfoMap, isPicking);
  }
}