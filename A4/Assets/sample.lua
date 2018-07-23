-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 10)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)
stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
green = gr.material({0.1, 0.55, 0.1}, {0.0, 0.0, 0.0}, 0)
trunkColor = gr.material({0.55, 0.33, 0.22}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)
gold = gr.material({1, 0.85, 0}, {0.5, 0.5, 0.5}, 25)

scene_root = gr.node('root')
scene_root:rotate('x', 10.0)
scene_root:scale(2, 2, 2)

leftPillar = gr.mesh( 'rightPillar', 'cylinder.obj' )
scene_root:add_child(leftPillar)
leftPillar:set_material(stone)
leftPillar:scale(20, 120, 20)
leftPillar:translate(-60, -80, 0)

rightPillar = gr.mesh( 'rightPillar', 'cylinder.obj' )
scene_root:add_child(rightPillar)
rightPillar:set_material(stone)
rightPillar:scale(20, 120, 20)
rightPillar:translate(60, -80, 0)

slogan = gr.nh_sphere('slogan', {0, 0, 0}, 1)
scene_root:add_child(slogan)
slogan:set_material(mat2)
slogan:scale(80, 10, 10)
slogan:translate(0, 40, 0.2)

balloonLeft = gr.nh_sphere('balloon', {200, 50, -100}, 50)
scene_root:add_child(balloonLeft)
balloonLeft:set_material(mat4)
balloonLeft:scale(0.5, 0.6, 0.5)
balloonLeft:translate(30, 80, 0)

balloonRight = gr.nh_sphere('balloon', {-200, 50, -100}, 50)
scene_root:add_child(balloonRight)
balloonRight:set_material(mat1)
balloonRight:scale(0.5, 0.6, 0.5)
balloonRight:translate(-20, 60, 0)

mickey = gr.mesh( 'Mickey', 'mickey.obj' )
scene_root:add_child(mickey)
mickey:set_material(mat3)
mickey:rotate('x', 10.0)
mickey:rotate('y', -70.0)
mickey:rotate('z', -100.0)
mickey:scale(100, 100, 100)
mickey:translate(-45, 30, 1)

trunk = gr.nh_box('trunk', {0, 0, 0}, 1)
trunk:set_material(trunkColor) -- should be tree
trunk:scale(0.6,4,0.6)

tree = gr.node('tree')
leaf = gr.nh_sphere('leaf', {0, 0, 0}, 1)
tree:add_child(leaf)
leaf:set_material(green)
leaf:scale(1.5, 2.5, 1.5)
leaf:translate(0.25, 5, 0.1)
tree:add_child(trunk)

for i = 1, 4 do
   treeRow = gr.node('treeRow' .. i)
   treeRow:add_child(tree)
   scene_root:add_child(treeRow)
   treeRow:scale(12, 12, 12)
   treeRow:translate(-150 + 60 * i, -150, 18)
end

cow_poly = gr.mesh('cow', 'cow.obj')

cow_poly:set_material(gold)
cow_poly:rotate('y', -45.0)
cow_poly:scale(10, 10, 10)
cow_poly:translate(0.0, -135.0, 0.0)
scene_root:add_child(cow_poly)

--white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
white_light = gr.light({100.0, 400.0, 1000.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
--orange_light = gr.light({0.0, 200.0, 500.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'scene.png', 512, 512,
    {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.3, 0.3, 0.3}, {white_light})
