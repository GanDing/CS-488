-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.0, 1.0, 1.0}, {0.5, 0.7, 0.5}, {1.0, 0.0, 0.0}, 25, 0.0, 5.0)
mat2 = gr.material({0.59, 0.29, 0.0}, {0.5, 0.7, 0.5}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)
mat4 = gr.material({1.0, 1.0, 1.0}, {0.5, 0.4, 0.8}, {1.0, 0.0, 0.0}, 25, 0.0, 1.0)
floorpng = gr.texture("texture/floor.png", {0, 0, 0}, {1.0, 0.0, 0.0}, 0, 0.0, 1.0)
earth = gr.texture("texture/earthmap.png", {0, 0, 0}, {1.0, 0.0, 0.0}, 0, 0.0, 1)
earthBump = gr.bump("texture/earthbump.png")
earth:set_bump(earthBump)

ball1 = gr.texture("texture/num11.png", {0, 0, 0}, {1.0, 0.0, 0.0}, 0, 0.0, 1.0)
ball2 = gr.texture("texture/num5.png", {0, 0, 0}, {1.0, 0.0, 0.0}, 0, 0.0, 1.0)

box = gr.texture("texture/box.png", {0, 0, 0}, {1.0, 0.0, 0.0}, 0, 0.0, 1.0)

scene_root = gr.node('root')

floor = gr.nh_box('floor', {0, -1200, -500}, 1200)
floor:translate(-400, -300, -500)
scene_root:add_child(floor)
floor:set_material(floorpng)

bed = gr.nh_box('bed', {0, 0, 0}, 1)
bed:scale(200, 50, 300)
bed:translate(0, -230, -400)
bed:rotate('y', -45)
scene_root:add_child(bed)
bed:set_material(mat1)

b1 = gr.nh_box('b1', {0, 0, 0}, 1)
b1:scale(100, 100, 100)
b1:translate(-200, -300, -300)
scene_root:add_child(b1)
b1:set_material(box)
b1:move(1, {0.5, 0.5, 0}, 0.5)

c1 = gr.cylinder('c1')
c1:scale(25, 30, 25)
c1:translate(230, -260, 20)
c1:set_material(mat2)
scene_root:add_child(c1)

c2 = gr.cylinder('c2')
c2:scale(25, 30, 25)
c2:translate(100, -260, -80)
c2:set_material(mat2)
scene_root:add_child(c2)


c3 = gr.cylinder('c3')
c3:scale(25, 30, 25)
c3:translate(400, -260, -150)
c3:set_material(mat2)
scene_root:add_child(c3)

s3 = gr.nh_sphere('s4', {-400, -50, -100}, 50)
s3:translate(150, -200, 50)
scene_root:add_child(s3)
s3:set_material(ball2)

s4 = gr.nh_sphere('s4', {-400, -50, -100}, 50)
s4:translate(200, -200, 0)
scene_root:add_child(s4)
s4:set_material(ball1)

s5 = gr.nh_sphere('s5', {0, 100, -250}, 150)
scene_root:add_child(s5)
s5:set_material(earth)

white_light = gr.light({100.0, 150.0, 200.0}, {0.9, 0.9, 0.9}, {1, 1, 1})

gr.render(scene_root, 'final.png', 512, 512,
    {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.3, 0.3, 0.3}, {white_light})

