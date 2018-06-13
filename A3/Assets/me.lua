-- puppet.lua
-- A simplified puppet without posable joints, but that
-- looks roughly humanoid.

rootnode = gr.node('root')
rootnode:rotate('y', -20.0)
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('cube', 'torso')
rootnode:add_child(torso)
torso:set_material(white)
torso:scale(0.5, 1.0, 0.5);
torso:translate(0, 0.3, 0)

torsoNeckJoint = gr.joint("torsoNeckJoint", {-15, 0, 15}, {0, 0, 0})
torsoNeckJoint:translate(0.0, 0.6, 0)
torso:add_child(torsoNeckJoint)
torsoNeckJoint:scale(1/0.5, 1, 1/0.5)

neck = gr.mesh('sphere', 'neck')
torsoNeckJoint:add_child(neck)
neck:scale(0.15, 0.3, 0.15)
neck:set_material(blue)

neckHeadJoint = gr.joint("neckHeadJoint", {-15, 0, 15}, {-80, 0, 80})
neckHeadJoint:translate(0.0, 0.33, 0.0)
neckHeadJoint:scale(1/0.15,1/0.3,1/0.15)
neck:add_child(neckHeadJoint)

head = gr.mesh('cube', 'head')
neckHeadJoint:add_child(head)
head:scale(0.4, 0.4, 0.4)
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

leftShoulder = gr.mesh('sphere', 'leftShoulder')
torso:add_child(leftShoulder)
leftShoulder:scale(1/0.5, 1, 1/0.5)
leftShoulder:scale(0.15, 0.15, 0.15)
leftShoulder:translate(-0.6, 0.45, 0.0)
leftShoulder:set_material(blue)

leftShoulderJoint = gr.joint("leftShoulderJoint", {-160, 0, 75}, {0, 0, 0})
leftShoulder:add_child(leftShoulderJoint)
leftShoulderJoint:scale(1/0.15, 1/0.15, 1/0.15)

leftArm = gr.mesh('cube', 'leftArm')
leftShoulderJoint:add_child(leftArm)
leftArm:scale(0.1, 0.6, 0.1)
leftArm:translate(-0.05, -0.4, 0.0)
leftArm:set_material(red)

leftElbow = gr.mesh('sphere', 'leftElbow')
leftArm:add_child(leftElbow)
leftElbow:scale(1/0.1, 1/0.6, 1/0.1)
leftElbow:scale(0.1, 0.1, 0.08)
leftElbow:translate(0, -0.22*(1/0.55), 0)
leftElbow:set_material(green)

leftElbowJoint = gr.joint('leftElbowJoint', {-140, 0, 0}, {0, 0, 0})
leftElbow:add_child(leftElbowJoint)
leftElbowJoint:scale(1/0.1, 1/0.1, 1/0.08)

leftForeArm = gr.mesh('cube', 'leftForeArm')
leftElbowJoint:add_child(leftForeArm)
leftForeArm:scale(0.1, 0.55, 0.1)
leftForeArm:translate(0, -0.25, 0.0)
leftForeArm:set_material(red)

leftWrist = gr.mesh('sphere', 'leftWrist')
leftForeArm:add_child(leftWrist)
leftWrist:scale(1/0.1, 1/0.5, 1/0.1)
leftWrist:scale(0.02, 0.02, 0.02)
leftWrist:translate(0, -0.20*2, 0)
leftWrist:set_material(green)

leftWristJoint = gr.joint('leftWristJoint', {-80, 0, 0}, {0, 0, 0})
leftWrist:add_child(leftWristJoint)
leftWristJoint:scale(1/0.02,1/0.02,1/0.02)

leftHand = gr.mesh('sphere', 'leftHand')
leftWristJoint:add_child(leftHand)
leftHand:scale(0.05, 0.05, 0.05)
leftHand:translate(0, -0.06, 0)
leftHand:set_material(green)



rightShoulder = gr.mesh('sphere', 'rightShoulder')
torso:add_child(rightShoulder)
rightShoulder:scale(1/0.5, 1, 1/0.5)
rightShoulder:scale(0.15, 0.15, 0.15)
rightShoulder:translate(0.6, 0.45, 0.0)
rightShoulder:set_material(blue)


rightShoulderJoint = gr.joint("rightShoulderJoint", {-160, 0, 75}, {0, 0, 0})
rightShoulder:add_child(rightShoulderJoint)
rightShoulderJoint:scale(1/0.15, 1/0.15, 1/0.15)


rightArm = gr.mesh('cube', 'rightArm')
rightShoulderJoint:add_child(rightArm)
rightArm:scale(0.1, 0.6, 0.1)
rightArm:translate(0.05, -0.4, 0.0)
rightArm:set_material(red)


rightElbow = gr.mesh('sphere', 'rightElbow')
rightArm:add_child(rightElbow)
rightElbow:scale(1/0.1, 1/0.6, 1/0.1)
rightElbow:scale(0.1, 0.1, 0.08)
rightElbow:translate(0, -0.22*(1/0.55), 0)
rightElbow:set_material(green)


rightElbowJoint = gr.joint('rightElbowJoint', {-140, 0, 0}, {0, 0, 0})
rightElbow:add_child(rightElbowJoint)
rightElbowJoint:scale(1/0.1, 1/0.1, 1/0.08)


rightForeArm = gr.mesh('cube', 'rightForeArm')
rightElbowJoint:add_child(rightForeArm)
rightForeArm:scale(0.1, 0.55, 0.1)
rightForeArm:translate(0, -0.25, 0.0)
rightForeArm:set_material(red)



rightWrist = gr.mesh('sphere', 'rightWrist')
rightForeArm:add_child(rightWrist)
rightWrist:scale(1/0.1, 1/0.5, 1/0.1)
rightWrist:scale(0.02, 0.02, 0.02)
rightWrist:translate(0, -0.22*2, 0)
rightWrist:set_material(green)


rightWristJoint = gr.joint('rightWristJoint', {-80, 0, 0}, {0, 0, 0})
rightWrist:add_child(rightWristJoint)
rightWristJoint:scale(1/0.02, 1/0.02, 1/0.02)



rightHand = gr.mesh('sphere', 'rightHand')
rightWristJoint:add_child(rightHand)
rightHand:scale(0.05, 0.05, 0.05)
rightHand:translate(0, -0.06, 0)
rightHand:set_material(green)


hip = gr.mesh('sphere', 'hip')
torso:add_child(hip)
hip:scale(1/0.5, 1, 1/0.5)
hip:scale(0.3, 0.2, 0.35)
hip:translate(0, -0.5, 0.0)
hip:set_material(blue)


leftHipJoint = gr.joint('leftHipJoint', {-90, 0, 90}, {0, 0, 0})
hip:add_child(leftHipJoint)
leftHipJoint:scale(1/0.3, 1/0.2, 1/0.35)
leftHipJoint:translate(-0.3, 0, 0.0)

leftThigh = gr.mesh('cube', 'leftThigh')
leftHipJoint:add_child(leftThigh)
leftThigh:scale(0.1, 0.6, 0.1)
leftThigh:translate(0, -0.35, 0)
leftThigh:set_material(red)

leftKnee = gr.mesh('sphere', 'leftKnee')
leftThigh:add_child(leftKnee)
leftKnee:scale(1/0.15, 1/0.6, 1/0.15)
leftKnee:scale(0.1, 0.12, 0.12)
leftKnee:translate(0, -0.45, 0)
leftKnee:set_material(green)

leftKneeJoint = gr.joint('leftKneeJoint', {0, 0, 130}, {0, 0, 0})
leftKnee:add_child(leftKneeJoint)
leftKneeJoint:scale(1/0.1, 1/0.12, 1/0.12)

leftCalf = gr.mesh('cube', 'leftCalf')
leftKneeJoint:add_child(leftCalf)
leftCalf:scale(0.15, 0.75, 0.15)
leftCalf:translate(0, -0.4, 0)
leftCalf:set_material(red)

LeftAnkleJoint = gr.joint('LeftAnkleJoint', {-30, -90, 0}, {-20, 0, 20})
leftCalf:add_child(LeftAnkleJoint)
LeftAnkleJoint:translate(0, -0.35, 0)
LeftAnkleJoint:scale(1/0.15, 1/0.75, 1/0.15)

leftFoot = gr.mesh('cube', 'leftFoot')
LeftAnkleJoint:add_child(leftFoot)
leftFoot:scale(0.15, 0.05, 0.5)
leftFoot:translate(0, 0, 0.15)
leftFoot:set_material(green)


rightHipJoint = gr.joint('rightHipJoint', {-90,0,90}, {0,0,0})
hip:add_child(rightHipJoint)
rightHipJoint:scale(1/0.3, 1/0.2, 1/0.35)
rightHipJoint:translate(0.3, 0, 0.0)

rightThigh = gr.mesh('cube', 'rightThigh')
rightHipJoint:add_child(rightThigh)
rightThigh:scale(0.1, 0.6, 0.1)
rightThigh:translate(0, -0.35, 0)
rightThigh:set_material(red)

rightKnee = gr.mesh('sphere', 'rightKnee')
rightThigh:add_child(rightKnee)
rightKnee:scale(1/0.15, 1/0.6, 1/0.15)
rightKnee:scale(0.1, 0.12, 0.12)
rightKnee:translate(0, -0.45, 0)
rightKnee:set_material(green)

rightKneeJoint = gr.joint('rightKneeJoint', {0, 0, 130}, {0, 0, 0})
rightKnee:add_child( rightKneeJoint)
rightKneeJoint:scale(1/0.1, 1/0.12, 1/0.12)

rightCalf = gr.mesh('cube', 'rightCalf')
rightKneeJoint:add_child(rightCalf)
rightCalf:scale(0.15, 0.75, 0.15)
rightCalf:translate(0, -0.4, 0)
rightCalf:set_material(red)

rightAnkleJoint = gr.joint('rightAnkleJoint', {-30, -90, 0}, {-20, 0, 20})
  rightCalf:add_child(rightAnkleJoint)
rightAnkleJoint:translate(0, -0.35, 0)
rightAnkleJoint:scale(1/0.15, 1/0.75, 1/0.15)

rightFoot = gr.mesh('cube', 'rightFoot')
rightAnkleJoint:add_child(rightFoot)
rightFoot:scale(0.15, 0.05, 0.5)
rightFoot:translate(0, 0, 0.15)
rightFoot:set_material(green)

return rootnode
