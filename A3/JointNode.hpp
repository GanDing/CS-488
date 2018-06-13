#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
  JointNode(const std::string & name);
  virtual ~JointNode();

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  void start_rotate(glm::vec3 angle);

  glm::vec3 reason_rotate(glm::vec3 rotate_angle);

  virtual glm::mat4 get_M();

  void update_rotate_angle(glm::vec3 & result_angle);

  
  struct JointRange {
    double min, init, max;
  };

  glm::vec3 previous_angle;
  glm::vec3 current_angle;



  JointRange m_joint_x, m_joint_y;
};