-- puppet.lua
-- A simplified puppet without posable joints, but that
-- looks roughly humanoid.

rootnode = gr.node('root')
rootnode:rotate('y', 20.0)
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('cube', 'torso')
  rootnode:add_child(torso)
torso:set_material(white)
torso:scale(0.5,1.0,0.5);
torso:translate(0,0.3,0)

torsoNeckJoint = gr.joint("torsoNeckJoint", {-10, 0, 10}, {0, 0, 0})
torsoNeckJoint:translate(0.0, 0.55, 0)
  torso:add_child(torsoNeckJoint)
torsoNeckJoint:scale(1/0.5,1,1/0.5)

neck = gr.mesh('sphere', 'neck')
  torsoNeckJoint:add_child(neck)
-- neck:scale(1.0/0.5, 1.0, 1.0/0.5)
neck:scale(0.15, 0.3, 0.15)
-- neck:translate(0.0, 0.6, 0.0)
-- neck:scale(0.12, 0.25, 0.12)
neck:set_material(blue)

neckHeadJoint = gr.joint("neckHeadJoint", {-30, 0, 30}, {-80, 0, 80})
neckHeadJoint:translate(0.0, 0.33, 0.0)
neckHeadJoint:scale(1/0.15,1/0.3,1/0.15)
  neck:add_child(neckHeadJoint)

head = gr.mesh('cube', 'head')
  neckHeadJoint:add_child(head)
-- head:scale(1.0/0.15, 1/0.3, 1.0/0.15)
head:scale(0.4, 0.4, 0.4)
-- head:translate(0.0, 0.9, 0.0)
-- head:scale(0.4, 0.4, 0.4)
head:set_material(red)

ears = gr.mesh('sphere', 'ears')
head:add_child(ears)
ears:scale(1.2, 0.08, 0.08)
ears:set_material(red)
ears:set_material(blue)

leftEye = gr.mesh('cube', 'leftEye')
head:add_child(leftEye)
leftEye:scale(0.2, 0.1, 0.1)
leftEye:translate(-0.2, 0.2, 0.5)
leftEye:set_material(blue)

rightEye = gr.mesh('cube', 'rightEye')
head:add_child(rightEye)
rightEye:scale(0.2, 0.1, 0.1)
rightEye:translate(0.2, 0.2, 0.5)
rightEye:set_material(blue)

-- left arms and hands ---------------------------

leftShoulder = gr.mesh('sphere', 'leftShoulder')
  torso:add_child(leftShoulder)
leftShoulder:scale(1/0.5,1,1/0.5)
leftShoulder:scale(0.13, 0.13, 0.13)
leftShoulder:translate(-0.31*2, 0.45, 0.0)
leftShoulder:set_material(blue)

leftShoulderJoint = gr.joint("leftShoulderJoint", {-160, 0, 75}, {0, 0, 0})
  leftShoulder:add_child(leftShoulderJoint)
leftShoulderJoint:scale(1/0.13, 1/0.13, 1/0.13)
-- leftShoulderJoint:rotate('z',90)

leftUpperArm = gr.mesh('cube', 'leftUpperArm')
  leftShoulderJoint:add_child(leftUpperArm)
leftUpperArm:scale(0.1, 0.55, 0.1)
leftUpperArm:translate(0, -0.39, 0.0)
leftUpperArm:set_material(red)

leftElbow = gr.mesh('sphere', 'leftElbow')
  leftUpperArm:add_child(leftElbow)
leftElbow:scale(1/0.1, 1/0.55, 1/0.1)
leftElbow:scale(0.06,0.06,0.05)
leftElbow:translate(0,-0.22*(1/0.55),0)
leftElbow:set_material(red)

leftElbowJoint = gr.joint('leftElbowJoint', {-140, 0, 0}, {0, 0, 0})
  leftElbow:add_child(leftElbowJoint)
leftElbowJoint:scale(1/0.06,1/0.06,1/0.05)

leftLowerArm = gr.mesh('cube', 'leftLowerArm')
  leftElbowJoint:add_child(leftLowerArm)
leftLowerArm:scale(0.1, 0.5, 0.1)
leftLowerArm:translate(0, -0.5/2, 0.0)
leftLowerArm:set_material(red)

leftWrist = gr.mesh('sphere', 'leftWrist')
  leftLowerArm:add_child(leftWrist)
leftWrist:scale(1/0.1, 1/0.5, 1/0.1)
leftWrist:scale(0.02,0.02,0.02)
leftWrist:translate(0,-0.27*2,0)
leftWrist:set_material(blue)

leftWristJoint = gr.joint('leftWristJoint', {-80, 0, 80}, {0, 0, 0})
  leftWrist:add_child(leftWristJoint)
leftWristJoint:scale(1/0.02,1/0.02,1/0.02)

leftHand = gr.mesh('cube', 'leftHand')
  leftWristJoint:add_child(leftHand)
leftHand:scale(0.05, 0.13, 0.13)
leftHand:translate(0,-0.05,0)
leftHand:set_material(green)


-- right arms and hands ---------------------------

rightShoulder = gr.mesh('sphere', 'rightShoulder')
  torso:add_child(rightShoulder)
rightShoulder:scale(1/0.5,1,1/0.5)
rightShoulder:scale(0.13, 0.13, 0.13)
rightShoulder:translate(0.31*2, 0.45, 0.0)
rightShoulder:set_material(blue)

rightShoulderJoint = gr.joint("rightShoulderJoint", {-160, 0, 75}, {0, 0, 0})
  rightShoulder:add_child(rightShoulderJoint)
rightShoulderJoint:scale(1/0.13, 1/0.13, 1/0.13)
-- rightShoulderJoint:rotate('z',90)

rightUpperArm = gr.mesh('cube', 'rightUpperArm')
  rightShoulderJoint:add_child(rightUpperArm)
rightUpperArm:scale(0.1, 0.55, 0.1)
rightUpperArm:translate(0, -0.39, 0.0)
rightUpperArm:set_material(red)

rightElbow = gr.mesh('sphere', 'rightElbow')
  rightUpperArm:add_child(rightElbow)
rightElbow:scale(1/0.1, 1/0.55, 1/0.1)
rightElbow:scale(0.06,0.06,0.05)
rightElbow:translate(0,-0.22*(1/0.55),0)
rightElbow:set_material(red)

rightElbowJoint = gr.joint('rightElbowJoint', {-140, 0, 0}, {0, 0, 0})
  rightElbow:add_child(rightElbowJoint)
rightElbowJoint:scale(1/0.06,1/0.06,1/0.05)

rightLowerArm = gr.mesh('cube', 'rightLowerArm')
  rightElbowJoint:add_child(rightLowerArm)
rightLowerArm:scale(0.1, 0.5, 0.1)
rightLowerArm:translate(0, -0.5/2, 0.0)
rightLowerArm:set_material(red)

rightWrist = gr.mesh('sphere', 'rightWrist')
  rightLowerArm:add_child(rightWrist)
rightWrist:scale(1/0.1, 1/0.5, 1/0.1)
rightWrist:scale(0.02,0.02,0.02)
rightWrist:translate(0,-0.27*2,0)
rightWrist:set_material(blue)

rightWristJoint = gr.joint('rightWristJoint', {-80, 0, 80}, {0, 0, 0})
  rightWrist:add_child(rightWristJoint)
rightWristJoint:scale(1/0.02,1/0.02,1/0.02)

rightHand = gr.mesh('cube', 'rightHand')
  rightWristJoint:add_child(rightHand)
rightHand:scale(0.05, 0.13, 0.13)
rightHand:translate(0,-0.05,0)
rightHand:set_material(green)


hip = gr.mesh('cube', 'hip')
  torso:add_child(hip)
hip:scale(1/0.5,1,1/0.5)
hip:scale(0.51, 0.2, 0.51)
hip:translate(0, -0.5, 0.0)
hip:set_material(blue)



-- left legs ------------------------------------
leftHipJoint = gr.joint('leftHipJoint', {-90,0,90}, {0,0,0})
  hip:add_child(leftHipJoint)
leftHipJoint:scale(1/0.51, 1/0.2, 1/0.51)
leftHipJoint:translate(-0.3, 0, 0.0)

leftUpperLeg = gr.mesh('cube', 'leftUpperLeg')
  leftHipJoint:add_child(leftUpperLeg)
leftUpperLeg:scale(0.15, 0.6, 0.15)
leftUpperLeg:translate(0,-0.37,0)
leftUpperLeg:set_material(red)

leftKnee = gr.mesh('sphere', 'leftKnee')
  leftUpperLeg:add_child(leftKnee)
leftKnee:scale(1/0.15, 1/0.6, 1/0.15)
leftKnee:scale(0.07, 0.08, 0.08)
leftKnee:translate(0,-0.46,0)
leftKnee:set_material(red)

leftKneeJoint = gr.joint('leftKneeJoint', {0,0,130}, {0,0,0})
  leftKnee:add_child( leftKneeJoint)
leftKneeJoint:scale(1/0.07, 1/0.09, 1/0.1)

leftLowerLeg = gr.mesh('cube', 'leftLowerLeg')
  leftKneeJoint:add_child(leftLowerLeg)
leftLowerLeg:scale(0.14, 0.75, 0.14)
leftLowerLeg:translate(0,-0.38,0)
leftLowerLeg:set_material(red)

LeftAnkleJoint = gr.joint('LeftAnkleJoint', {-100,-90,0}, {0,0,0})
  leftLowerLeg:add_child(LeftAnkleJoint)
LeftAnkleJoint:translate(0,-0.34,0)
LeftAnkleJoint:scale(1/0.14, 1/0.75, 1/0.14)

leftFoot = gr.mesh('cube', 'leftFoot')
  LeftAnkleJoint:add_child(leftFoot)
leftFoot:scale(0.141, 0.3, 0.1)
leftFoot:translate(0,-0.1,0)
leftFoot:set_material(green)




-- right legs ------------------------------------
rightHipJoint = gr.joint('rightHipJoint', {-90,0,90}, {0,0,0})
  hip:add_child(rightHipJoint)
rightHipJoint:scale(1/0.51, 1/0.2, 1/0.51)
rightHipJoint:translate(0.3, 0, 0.0)

rightUpperLeg = gr.mesh('cube', 'rightUpperLeg')
  rightHipJoint:add_child(rightUpperLeg)
rightUpperLeg:scale(0.15, 0.6, 0.15)
rightUpperLeg:translate(0,-0.37,0)
rightUpperLeg:set_material(red)

rightKnee = gr.mesh('sphere', 'rightKnee')
  rightUpperLeg:add_child(rightKnee)
rightKnee:scale(1/0.15, 1/0.6, 1/0.15)
rightKnee:scale(0.07, 0.08, 0.08)
rightKnee:translate(0,-0.46,0)
rightKnee:set_material(red)

rightKneeJoint = gr.joint('rightKneeJoint', {0,0,130}, {0,0,0})
  rightKnee:add_child( rightKneeJoint)
rightKneeJoint:scale(1/0.07, 1/0.09, 1/0.1)

rightLowerLeg = gr.mesh('cube', 'rightLowerLeg')
  rightKneeJoint:add_child(rightLowerLeg)
rightLowerLeg:scale(0.14, 0.75, 0.14)
rightLowerLeg:translate(0,-0.38,0)
rightLowerLeg:set_material(red)

rightAnkleJoint = gr.joint('rightAnkleJoint', {-100,-90,0}, {0,0,0})
  rightLowerLeg:add_child(rightAnkleJoint)
rightAnkleJoint:translate(0,-0.34,0)
rightAnkleJoint:scale(1/0.14, 1/0.75, 1/0.14)

rightFoot = gr.mesh('cube', 'rightFoot')
  rightAnkleJoint:add_child(rightFoot)
rightFoot:scale(0.141, 0.3, 0.1)
rightFoot:translate(0,-0.1,0)
rightFoot:set_material(green)

-- torsoNeckJoint:rotate('x',-10)
-- torsoNeckJoint:rotate('z',90)

-- leftShoulderJoint:rotate('y',0)
-- -- leftElbowJoint:rotate('y',90)
-- leftWristJoint:rotate('x',00)
-- neckHeadJoint:rotate('x',30)
-- neckHeadJoint:rotate('x',00)

return rootnode
