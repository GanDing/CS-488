#include "JointNode.hpp"
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/io.hpp>
#include "cs488-framework/MathUtils.hpp"

using namespace glm;


//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
  m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}

glm::vec3 JointNode::reason_rotate(glm::vec3 rotate_angle) {
  glm::vec3 reason_angle = glm::vec3();

  reason_angle.x = rotate_angle.x;

  if (previous_angle.x + rotate_angle.x > m_joint_x.max) 
    reason_angle.x = m_joint_x.max - previous_angle.x;
  if (previous_angle.x + rotate_angle.x < m_joint_x.min)
    reason_angle.x = m_joint_x.min - previous_angle.x;

  reason_angle.y = rotate_angle.y;

  if (previous_angle.y + rotate_angle.y > m_joint_y.max) 
    reason_angle.y = m_joint_y.max - previous_angle.y;
  if (previous_angle.x + rotate_angle.y < m_joint_y.min)
    reason_angle.y = m_joint_y.min - previous_angle.y;

  reason_angle.z = rotate_angle.z;

  return reason_angle;
}

glm::mat4 JointNode::get_M() {
  glm::vec3 total_angle = previous_angle + current_angle;

  glm::mat4 rotate_mat_x = glm::rotate(degreesToRadians(total_angle.x), vec3(1,0,0));
  glm::mat4 rotate_mat_y = glm::rotate(degreesToRadians(total_angle.y), vec3(0,1,0));
  glm::mat4 rotate_mat_z = glm::rotate(degreesToRadians(total_angle.z), vec3(0,0,1));
  
  glm::mat4 rotate_mat = rotate_mat_x * rotate_mat_y * rotate_mat_z;

  return parent_trans * trans * rotate_mat;
}

void JointNode::update_rotate_angle(vec3 & result_angle) {
  previous_angle = previous_angle + current_angle;
  result_angle = current_angle;
  current_angle = vec3();
}

void JointNode::start_rotate(glm::vec3 angle) {
  current_angle = reason_rotate(angle);
  apply_effect_to_child();
}

 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}